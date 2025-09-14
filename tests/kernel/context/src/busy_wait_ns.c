
#include <stdbool.h>
#include <stdint.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/time_units.h>
#include <zephyr/sys/util.h>
#include <zephyr/ztest.h>

#define N 50

/* Forward declare an array of a pseudo-random nanosecond values within the range [0, 100ms] */
static const uint32_t delay_nsecs[N];

static uint32_t measured_cycles[N];

void busy_wait_thread_ns(void *semp, void *arg2, void *arg3)
{
	ARRAY_FOR_EACH(delay_nsecs, i) {
        /*
         * Note: this is going to be a very bad measurement for small values, especially when
         * clocks are sub 1 GHz.
         */
        register uint32_t nsecs = delay_nsecs[i];
        register uint32_t cycles;

        cycles = k_cycle_get_32();
		k_busy_wait_ns(nsecs);
        cycles = k_cycle_get_32() - cycles;

        measured_cycles[i] = cycles;
	}

	k_sem_give(semp);
}

/*
 * If measured_nsecs were plotted as a function of delay_nsecs, the expectation is that we would
 * observe something similar to a step function until delay_nsecs exceeds some value above a base
 * [nsec / cycle] value.
 *
 * Beyond that, we expect that measured_nsecs would be approximately linear with respect to
 * delay_nsecs, so that the correlation coefficient should be approximately 1.0.
 */
void busy_wait_ns_check_results(void)
{
    printk("cycles per second: %u\n", sys_clock_hw_cycles_per_sec());

    ARRAY_FOR_EACH(delay_nsecs, i) {
        uint32_t measured_nsecs = (uint32_t)k_cyc_to_ns_ceil64(measured_cycles[i]);
        int32_t error_nsecs = (int32_t)measured_nsecs - (int32_t)delay_nsecs[i];

        // printk("%2zu: expect: %10u actual: %10u error: %d\n", i,
        //        delay_nsecs[i], measured_nsecs, error_nsecs);
        //zexpect_true(error_nsecs >= 0);

        printk("%u,%u\n", delay_nsecs[i], measured_nsecs);
    }
}

static const uint32_t delay_nsecs[N] = {
    /*
     * Uniform random distributions don't tend to do a great job at covering specific points of
     * interest in the range, so we manually include a few specific values to ensure we cover the
     * edge cases, and then fill in the rest with random values.
     */
    0,
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    /* pseudo-random sequence generated with
     * while true; do
     *   SET=($(shuf -i 0-100000000 -n 40))
     *   SUM=0
     *   for i in ${SET[@]}; do
     *     SUM=$((SUM+i))
     *   done
     *   AVG=$((SUM/40))
     *   if [ $AVG -lt 48000000 ]; then
     *     break
     *   fi
     * done
     * for i in ${SET[@]}; do
     *   echo "$i,"
     * done
     * echo AVG=$AVG
     */
    54258167,
    56633646,
    82529349,
    92738988,
    44876655,
    44630578,
    77921689,
    75330035,
    79816869,
    98886618,
    47809011,
    74669005,
    9179002,
    10357280,
    186951,
    86819660,
    94683785,
    79806281,
    19481404,
    96807939,
    50227353,
    53538751,
    45182541,
    16072053,
    93927827,
    87239923,
    38150233,
    57923217,
    27782014,
    81300982,
    23246611,
    59136798,
    11860084,
    18967943,
    22924683,
    50542644,
    33690187,
    76911122,
    61432438,
    52302833,
    /* AVG=45620482 */
};
