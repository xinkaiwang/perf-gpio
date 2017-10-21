
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <getopt.h>
#include <math.h>

#include "mailbox.h"

#define DMY 255 // Used to represent an invalid P1 pin, or unmapped servo

// #define NUM_P1PINS  40
// #define NUM_P5PINS  8

#define MAX_SERVOS  32 
                 /* Only 21 really, but this lets you map servo IDs
                 * to P1 pins, if you want to
                 */
#define MAX_MEMORY_USAGE    (16*1024*1024)  /* Somewhat arbitrary limit of 16MB */

#define DEFAULT_CYCLE_TIME_US   20000
#define DEFAULT_STEP_TIME_US    100
#define DEFAULT_SERVO_MIN_US    0
#define DEFAULT_SERVO_MAX_US    20000
// #define DEFAULT_STEP_TIME_US    10
// #define DEFAULT_SERVO_MIN_US    500
// #define DEFAULT_SERVO_MAX_US    2500

#define PAGE_SIZE       4096
#define PAGE_SHIFT      12

#define DMA_CHAN_SIZE       0x100
#define DMA_CHAN_MIN        0
#define DMA_CHAN_MAX        14
#define DMA_CHAN_DEFAULT    14

#define DMA_BASE_OFFSET     0x00007000
#define DMA_LEN         DMA_CHAN_SIZE * (DMA_CHAN_MAX+1)
#define PWM_BASE_OFFSET     0x0020C000
#define PWM_LEN         0x28
#define CLK_BASE_OFFSET         0x00101000
#define CLK_LEN         0xA8
#define GPIO_BASE_OFFSET    0x00200000
#define GPIO_LEN        0x100
#define PCM_BASE_OFFSET     0x00203000
#define PCM_LEN         0x24

#define DMA_VIRT_BASE       (periph_virt_base + DMA_BASE_OFFSET)
#define PWM_VIRT_BASE       (periph_virt_base + PWM_BASE_OFFSET)
#define CLK_VIRT_BASE       (periph_virt_base + CLK_BASE_OFFSET)
#define GPIO_VIRT_BASE      (periph_virt_base + GPIO_BASE_OFFSET)
#define PCM_VIRT_BASE       (periph_virt_base + PCM_BASE_OFFSET)

#define PWM_PHYS_BASE       (periph_phys_base + PWM_BASE_OFFSET)
#define PCM_PHYS_BASE       (periph_phys_base + PCM_BASE_OFFSET)
#define GPIO_PHYS_BASE      (periph_phys_base + GPIO_BASE_OFFSET)

#define DMA_NO_WIDE_BURSTS  (1<<26)
#define DMA_WAIT_RESP       (1<<3)
#define DMA_D_DREQ      (1<<6)
#define DMA_PER_MAP(x)      ((x)<<16)
#define DMA_END         (1<<1)
#define DMA_RESET       (1<<31)
#define DMA_INT         (1<<2)

#define DMA_CS          (0x00/4)
#define DMA_CONBLK_AD       (0x04/4)
#define DMA_SOURCE_AD       (0x0c/4)
#define DMA_DEBUG       (0x20/4)

#define GPIO_FSEL0      (0x00/4)
#define GPIO_SET0       (0x1c/4)
#define GPIO_CLR0       (0x28/4)
#define GPIO_LEV0       (0x34/4)
#define GPIO_PULLEN     (0x94/4)
#define GPIO_PULLCLK        (0x98/4)

#define GPIO_MODE_IN        0
#define GPIO_MODE_OUT       1

#define PWM_CTL         (0x00/4)
#define PWM_DMAC        (0x08/4)
#define PWM_RNG1        (0x10/4)
#define PWM_FIFO        (0x18/4)

#define PWMCLK_CNTL     40
#define PWMCLK_DIV      41

#define PWMCTL_MODE1        (1<<1)
#define PWMCTL_PWEN1        (1<<0)
#define PWMCTL_CLRF     (1<<6)
#define PWMCTL_USEF1        (1<<5)

#define PWMDMAC_ENAB        (1<<31)
#define PWMDMAC_THRSHLD     ((15<<8)|(15<<0))

#define PCM_CS_A        (0x00/4)
#define PCM_FIFO_A      (0x04/4)
#define PCM_MODE_A      (0x08/4)
#define PCM_RXC_A       (0x0c/4)
#define PCM_TXC_A       (0x10/4)
#define PCM_DREQ_A      (0x14/4)
#define PCM_INTEN_A     (0x18/4)
#define PCM_INT_STC_A       (0x1c/4)
#define PCM_GRAY        (0x20/4)

#define PCMCLK_CNTL     38
#define PCMCLK_DIV      39

#define DELAY_VIA_PWM       0
#define DELAY_VIA_PCM       1

#define ROUNDUP(val, blksz) (((val)+((blksz)-1)) & ~(blksz-1))

typedef struct {
    uint32_t info, src, dst, length,
         stride, next, pad[2];
} dma_cb_t;

#define BUS_TO_PHYS(x) ((x)&~0xC0000000)

// cycle_time_us is the pulse cycle time per servo, in microseconds.
// Typically it should be 20ms, or 20000us.

// step_time_us is the pulse width increment granularity, again in microseconds.
// Setting step_time_us too low will likely cause problems as the DMA controller
// will use too much memory bandwidth.  10us is a good value, though you
// might be ok setting it as low as 2us.


static int cycle_time_us;
static int step_time_us;

static uint8_t servo2gpio[MAX_SERVOS];
// static uint8_t p1pin2servo[NUM_P1PINS+1];
// static uint8_t p5pin2servo[NUM_P5PINS+1];
static int servostart[MAX_SERVOS];
static int servowidth[MAX_SERVOS];
static int num_servos;
static uint32_t gpiomode[MAX_SERVOS]; // store old gpio mode for all the pins we took over
static int restore_gpio_modes; // true means restore old gpio mode when terminate

static volatile uint32_t *pwm_reg;
static volatile uint32_t *pcm_reg;
static volatile uint32_t *clk_reg;
static volatile uint32_t *dma_reg;
static volatile uint32_t *gpio_reg;

static int delay_hw = DELAY_VIA_PWM;

static struct timeval *servo_kill_time;

static int dma_chan;
static int idle_timeout;
static int invert = 0;
static int servo_min_ticks;
static int servo_max_ticks;
static int num_samples;
static int num_cbs;
static int num_pages;
static uint32_t *turnoff_mask;
static uint32_t *turnon_mask;
static dma_cb_t *cb_base;

static int board_model;
static int gpio_cfg;

static uint32_t periph_phys_base;
static uint32_t periph_virt_base;
static uint32_t dram_phys_base;
static uint32_t mem_flag;

static struct {
    int handle;     /* From mbox_open() */
    uint32_t size;      /* Required size */
    unsigned mem_ref;   /* From mem_alloc() */
    unsigned bus_addr;  /* From mem_lock() */
    uint8_t *virt_addr; /* From mapmem() */
} mbox;

static void set_servo(int servo, int width);
// static void set_servo_idle(int servo);
static void gpio_set_mode(uint32_t gpio, uint32_t mode);
// static char *gpio2pinname(uint8_t gpio);

static void
udelay(int us)
{
    struct timespec ts = { 0, us * 1000 };

    nanosleep(&ts, NULL);
}

static void
terminate(int dummy)
{
    int i;

    if (dma_reg && mbox.virt_addr) {
        for (i = 0; i < MAX_SERVOS; i++) {
            if (servo2gpio[i] != DMY)
                set_servo(i, 0);
        }
        udelay(cycle_time_us);
        dma_reg[DMA_CS] = DMA_RESET;
        udelay(10);
    }
    if (restore_gpio_modes) {
        for (i = 0; i < MAX_SERVOS; i++) {
            if (servo2gpio[i] != DMY)
                gpio_set_mode(servo2gpio[i], gpiomode[i]);
        }
    }
    if (mbox.virt_addr != NULL) {
        unmapmem(mbox.virt_addr, mbox.size);
        mem_unlock(mbox.handle, mbox.mem_ref);
        mem_free(mbox.handle, mbox.mem_ref);
        if (mbox.handle >= 0)
            mbox_close(mbox.handle);
    }

    // unlink(DEVFILE);
    // unlink(CFGFILE);
    exit(1);
}

static void
fatal(char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    terminate(0);
}

static void
init_idle_timers(void)
{
    servo_kill_time = (struct timeval *)calloc(MAX_SERVOS, sizeof(struct timeval));
    if (!servo_kill_time)
        fatal("servod: calloc() failed\n");
}

static void
update_idle_time(int servo)
{
    if (idle_timeout == 0)
        return;

    gettimeofday(servo_kill_time + servo, NULL);
    servo_kill_time[servo].tv_sec += idle_timeout / 1000;
    servo_kill_time[servo].tv_usec += (idle_timeout % 1000) * 1000;
    while (servo_kill_time[servo].tv_usec >= 1000000) {
        servo_kill_time[servo].tv_usec -= 1000000;
        servo_kill_time[servo].tv_sec++;
    }
}

static void
gpio_set_mode(uint32_t gpio, uint32_t mode)
{
    uint32_t fsel = gpio_reg[GPIO_FSEL0 + gpio/10];

    fsel &= ~(7 << ((gpio % 10) * 3));
    fsel |= mode << ((gpio % 10) * 3);
    gpio_reg[GPIO_FSEL0 + gpio/10] = fsel;
}

static uint32_t
mem_virt_to_phys(void *virt)
{
    uint32_t offset = (uint8_t *)virt - mbox.virt_addr;

    return mbox.bus_addr + offset;
}


/* Carefully add or remove bits from the turnoff_mask such that regardless
 * of where the DMA controller is in its cycle, and whether we are increasing
 * or decreasing the pulse width, the generated pulse will only ever be the
 * old width or the new width.  If we don't take such care then there could be
 * a cycle with some pulse width between the two requested ones.  That doesn't
 * really matter for servos, but when driving LEDs some odd intensity for one
 * cycle can be noticeable.  It may be that the servo output has been turned
 * off via the inactivity timer, which is handled by always setting the turnon
 * mask appropriately at the end of this function.
 */
static void
set_servo(int servo, int width)
{
    volatile uint32_t *dp;
    int i;
    uint32_t mask = 1 << servo2gpio[servo];


    if (width > servowidth[servo]) {
        dp = turnoff_mask + servostart[servo] + width;
        if (dp >= turnoff_mask + num_samples)
            dp -= num_samples;

        for (i = width; i > servowidth[servo]; i--) {
            dp--;
            if (dp < turnoff_mask)
                dp = turnoff_mask + num_samples - 1;
            //printf("%5d, clearing at %p\n", dp - ctl->turnoff, dp);
            *dp &= ~mask;
        }
    } else if (width < servowidth[servo]) {
        dp = turnoff_mask + servostart[servo] + width;
        if (dp >= turnoff_mask + num_samples)
            dp -= num_samples;

        for (i = width; i < servowidth[servo]; i++) {
            //printf("%5d, setting at %p\n", dp - ctl->turnoff, dp);
            *dp++ |= mask;
            if (dp >= turnoff_mask + num_samples)
                dp = turnoff_mask;
        }
    }
    servowidth[servo] = width;
    if (width == 0) {
        turnon_mask[servo] = 0;
    } else {
        turnon_mask[servo] = mask;
    }
    update_idle_time(servo);
}

static void
setup_sighandlers(void)
{
    int i;

    // Catch all signals possible - it is vital we kill the DMA engine
    // on process exit!
    for (i = 0; i < 64; i++) {
        struct sigaction sa;

        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = terminate;
        sigaction(i, &sa, NULL);
    }
}

static void
init_ctrl_data(void)
{
    dma_cb_t *cbp = cb_base;
    uint32_t phys_fifo_addr, cbinfo;
    uint32_t phys_gpclr0;
    uint32_t phys_gpset0;
    int servo, i, numservos = 0, curstart = 0;
    uint32_t maskall = 0;

    if (invert) {
        phys_gpclr0 = GPIO_PHYS_BASE + 0x1c;
        phys_gpset0 = GPIO_PHYS_BASE + 0x28;
    } else {
        phys_gpclr0 = GPIO_PHYS_BASE + 0x28;
        phys_gpset0 = GPIO_PHYS_BASE + 0x1c;
    }

    if (delay_hw == DELAY_VIA_PWM) {
        phys_fifo_addr = PWM_PHYS_BASE + 0x18;
        cbinfo = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP | DMA_D_DREQ | DMA_PER_MAP(5);
    } else {
        phys_fifo_addr = PCM_PHYS_BASE + 0x04;
        cbinfo = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP | DMA_D_DREQ | DMA_PER_MAP(2);
    }

    memset(turnon_mask, 0, MAX_SERVOS * sizeof(*turnon_mask));

    for (servo = 0 ; servo < MAX_SERVOS; servo++) {
        servowidth[servo] = 0;
        if (servo2gpio[servo] != DMY) {
            numservos++;
            maskall |= 1 << servo2gpio[servo];
        }
    }

    for (i = 0; i < num_samples; i++)
        turnoff_mask[i] = maskall;

    for (servo = 0; servo < MAX_SERVOS; servo++) {
        if (servo2gpio[servo] != DMY) {
            servostart[servo] = curstart;
            curstart += num_samples / num_servos;
        }
    }

    servo = 0;
    while (servo < MAX_SERVOS && servo2gpio[servo] == DMY)
        servo++;

    for (i = 0; i < num_samples; i++) {
        cbp->info = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP;
        cbp->src = mem_virt_to_phys(turnoff_mask + i);
        cbp->dst = phys_gpclr0;
        cbp->length = 4;
        cbp->stride = 0;
        cbp->next = mem_virt_to_phys(cbp + 1);
        cbp++;
        if (i == servostart[servo]) {
            cbp->info = DMA_NO_WIDE_BURSTS | DMA_WAIT_RESP;
            cbp->src = mem_virt_to_phys(turnon_mask + servo);
            cbp->dst = phys_gpset0;
            cbp->length = 4;
            cbp->stride = 0;
            cbp->next = mem_virt_to_phys(cbp + 1);
            cbp++;
            servo++;
            while (servo < MAX_SERVOS && servo2gpio[servo] == DMY)
                servo++;
        }
        // Delay
        cbp->info = cbinfo;
        cbp->src = mem_virt_to_phys(turnoff_mask);  // Any data will do
        cbp->dst = phys_fifo_addr;
        cbp->length = 4;
        cbp->stride = 0;
        cbp->next = mem_virt_to_phys(cbp + 1);
        cbp++;
    }
    cbp--;
    cbp->next = mem_virt_to_phys(cb_base);
}

static void
init_hardware(void)
{
    if (delay_hw == DELAY_VIA_PWM) {
        // Initialise PWM
        pwm_reg[PWM_CTL] = 0;
        udelay(10);
        clk_reg[PWMCLK_CNTL] = 0x5A000006;      // Source=PLLD (500MHz)
        udelay(100);
        clk_reg[PWMCLK_DIV] = 0x5A000000 | (500<<12);   // set pwm div to 500, giving 1MHz
        udelay(100);
        clk_reg[PWMCLK_CNTL] = 0x5A000016;      // Source=PLLD and enable
        udelay(100);
        pwm_reg[PWM_RNG1] = step_time_us;
        udelay(10);
        pwm_reg[PWM_DMAC] = PWMDMAC_ENAB | PWMDMAC_THRSHLD;
        udelay(10);
        pwm_reg[PWM_CTL] = PWMCTL_CLRF;
        udelay(10);
        pwm_reg[PWM_CTL] = PWMCTL_USEF1 | PWMCTL_PWEN1;
        udelay(10);
    } else {
        // Initialise PCM
        pcm_reg[PCM_CS_A] = 1;              // Disable Rx+Tx, Enable PCM block
        udelay(100);
        clk_reg[PCMCLK_CNTL] = 0x5A000006;      // Source=PLLD (500MHz)
        udelay(100);
        clk_reg[PCMCLK_DIV] = 0x5A000000 | (500<<12);   // Set pcm div to 500, giving 1MHz
        udelay(100);
        clk_reg[PCMCLK_CNTL] = 0x5A000016;      // Source=PLLD and enable
        udelay(100);
        pcm_reg[PCM_TXC_A] = 0<<31 | 1<<30 | 0<<20 | 0<<16; // 1 channel, 8 bits
        udelay(100);
        pcm_reg[PCM_MODE_A] = (step_time_us - 1) << 10;
        udelay(100);
        pcm_reg[PCM_CS_A] |= 1<<4 | 1<<3;       // Clear FIFOs
        udelay(100);
        pcm_reg[PCM_DREQ_A] = 64<<24 | 64<<8;       // DMA Req when one slot is free?
        udelay(100);
        pcm_reg[PCM_CS_A] |= 1<<9;          // Enable DMA
        udelay(100);
    }

    // Initialise the DMA
    dma_reg[DMA_CS] = DMA_RESET;
    udelay(10);
    dma_reg[DMA_CS] = DMA_INT | DMA_END;
    dma_reg[DMA_CONBLK_AD] = mem_virt_to_phys(cb_base);
    dma_reg[DMA_DEBUG] = 7; // clear debug error flags
    dma_reg[DMA_CS] = 0x10880001;   // go, mid priority, wait for outstanding writes

    if (delay_hw == DELAY_VIA_PCM) {
        pcm_reg[PCM_CS_A] |= 1<<2;          // Enable Tx
    }
}

/* Determining the board revision is a lot more complicated than it should be
 * (see comments in wiringPi for details).  We will just look at the last two
 * digits of the Revision string and treat '00' and '01' as errors, '02' and
 * '03' as rev 1, and any other hex value as rev 2.  'Pi1 and Pi2 are
 * differentiated by the Hardware being BCM2708 or BCM2709.
 */
static void
get_model_and_revision(void)
{
    char buf[128], revstr[128], modelstr[128];
    char *ptr, *end, *res;
    int board_revision;
    FILE *fp;

    revstr[0] = modelstr[0] = '\0';

    fp = fopen("/proc/cpuinfo", "r");

    if (!fp)
        fatal("Unable to open /proc/cpuinfo: %m\n");

    while ((res = fgets(buf, 128, fp))) {
        if (!strncasecmp("hardware", buf, 8))
            memcpy(modelstr, buf, 128);
        else if (!strncasecmp(buf, "revision", 8))
            memcpy(revstr, buf, 128);
    }
    fclose(fp);

    if (modelstr[0] == '\0')
        fatal("servod: No 'Hardware' record in /proc/cpuinfo\n");
    if (revstr[0] == '\0')
        fatal("servod: No 'Revision' record in /proc/cpuinfo\n");

    if (strstr(modelstr, "BCM2708"))
        board_model = 1;
    else if (strstr(modelstr, "BCM2709"))
        board_model = 2;
    else if (strstr(modelstr, "BCM2835"))   /* Quick hack for Pi-Zero */
        board_model = 1;
    else
        fatal("servod: Cannot parse the hardware name string\n");

    ptr = revstr + strlen(revstr) - 3;
    board_revision = strtol(ptr, &end, 16);
    if (end != ptr + 2)
        fatal("servod: Failed to parse Revision string\n");
    if (board_revision < 1)
        fatal("servod: Invalid board Revision\n");
    else if (board_revision < 4)
        gpio_cfg = 1;
    else if (board_revision < 16)
        gpio_cfg = 2;
    else
        gpio_cfg = 3;

    if (board_model == 1) {
        periph_virt_base = 0x20000000;
        periph_phys_base = 0x7e000000;
        dram_phys_base   = 0x40000000;
        mem_flag         = 0x0c;
    } else {
        periph_virt_base = 0x3f000000;
        periph_phys_base = 0x7e000000;
        dram_phys_base   = 0xc0000000;
        mem_flag         = 0x04;
    }
}

static void
init_main() {
    memset(servo2gpio, DMY, sizeof(servo2gpio));

    get_model_and_revision();

    dma_chan = DMA_CHAN_DEFAULT;

    idle_timeout = 0;

    cycle_time_us = DEFAULT_CYCLE_TIME_US;

    step_time_us = DEFAULT_STEP_TIME_US;

    if (cycle_time_us % step_time_us) {
        fatal("cycle-time is not a multiple of step-size\n");
    }

    if (cycle_time_us / step_time_us < 100) {
        fatal("cycle-time must be at least 100 * step-size\n");
    }

    servo_min_ticks = DEFAULT_SERVO_MIN_US / step_time_us;

    servo_max_ticks = DEFAULT_SERVO_MAX_US / step_time_us;

    num_samples = cycle_time_us / step_time_us; // 20ms/0.1ms = 200
    num_cbs =     num_samples * 2 + MAX_SERVOS; // 200 * 2 + 32 = 432
    num_pages =   (num_cbs * sizeof(dma_cb_t) + num_samples * 4 +
                MAX_SERVOS * 4 + PAGE_SIZE - 1) >> PAGE_SHIFT; // pageSize=4096

    if (num_pages > MAX_MEMORY_USAGE / PAGE_SIZE) {
        fatal("Using too much memory; reduce cycle-time or increase step-size\n");
    }

    if (servo_max_ticks > num_samples) {
        fatal("max value is larger than cycle time\n");
    }
    if (servo_min_ticks >= servo_max_ticks) {
        fatal("min value is >= max value\n");
    }

    printf("\nBoard model:               %7d\n", board_model);
    printf("Using hardware:                %s\n", delay_hw == DELAY_VIA_PWM ? "PWM" : "PCM");
    printf("Using DMA channel:         %7d\n", dma_chan);
    if (idle_timeout)
        printf("Idle timeout:              %7dms\n", idle_timeout);
    else
        printf("Idle timeout:             Disabled\n");
    printf("Number of servos:          %7d\n", num_servos);
    printf("Servo cycle time:          %7dus\n", cycle_time_us);
    printf("Pulse increment step size: %7dus\n", step_time_us);
    printf("Minimum width value:       %7d (%dus)\n", servo_min_ticks,
                        servo_min_ticks * step_time_us);
    printf("Maximum width value:       %7d (%dus)\n", servo_max_ticks,
                        servo_max_ticks * step_time_us);
    printf("Output levels:            %s\n", invert ? "Inverted" : "  Normal");
    printf("\nServo mapping:\n");
    for (i = 0; i < MAX_SERVOS; i++) {
        if (servo2gpio[i] == DMY)
            continue;
        printf("    %2d          GPIO-%d\n", i, servo2gpio[i]);
    }
    printf("\n");

    init_idle_timers();
    setup_sighandlers();

    dma_reg = map_peripheral(DMA_VIRT_BASE, DMA_LEN);
    dma_reg += dma_chan * DMA_CHAN_SIZE / sizeof(uint32_t);
    pwm_reg = map_peripheral(PWM_VIRT_BASE, PWM_LEN);
    pcm_reg = map_peripheral(PCM_VIRT_BASE, PCM_LEN);
    clk_reg = map_peripheral(CLK_VIRT_BASE, CLK_LEN);
    gpio_reg = map_peripheral(GPIO_VIRT_BASE, GPIO_LEN);

    /* Use the mailbox interface to the VC to ask for physical memory */
    // Use the mailbox interface to request memory from the VideoCore
    // We specifiy (-1) for the handle rather than calling mbox_open()
    // so multiple users can share the resource.
    mbox.handle = -1; // mbox_open();
    mbox.size = num_pages * 4096;
    mbox.mem_ref = mem_alloc(mbox.handle, mbox.size, 4096, mem_flag);
    if (mbox.mem_ref < 0) {
        fatal("Failed to alloc memory from VideoCore\n");
    }
    mbox.bus_addr = mem_lock(mbox.handle, mbox.mem_ref);
    if (mbox.bus_addr == ~0) {
        mem_free(mbox.handle, mbox.size);
        fatal("Failed to lock memory\n");
    }
    mbox.virt_addr = mapmem(BUS_TO_PHYS(mbox.bus_addr), mbox.size);

    turnoff_mask = (uint32_t *)mbox.virt_addr;
    turnon_mask = (uint32_t *)(mbox.virt_addr + num_samples * sizeof(uint32_t));
    cb_base = (dma_cb_t *)(mbox.virt_addr +
        ROUNDUP(num_samples + MAX_SERVOS, 8) * sizeof(uint32_t));
}

