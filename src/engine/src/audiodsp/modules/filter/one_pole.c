#include "audiodsp/constants.h"
#include "audiodsp/lib/denormal.h"
#include "audiodsp/lib/pitch_core.h"
#include "audiodsp/modules/filter/one_pole.h"

/*void v_opl_set_coeff(
 * t_opl_one_pole* a_opl,
 * SGFLT a_cutoff //Cutoff in MIDI note number.  Typically 30 to 120
 * )
 */
void v_opl_set_coeff(t_opl_one_pole* a_opl, SGFLT a_cutoff)
{
    a_opl->cutoff = f_pit_midi_note_to_hz_fast(a_cutoff);
    a_opl->x = exp(-2.0f*PI*((a_opl->cutoff)*(a_opl->sr_recip)));
    a_opl->a0 = 1.0f-(a_opl->x);
    a_opl->b1 = -(a_opl->x);
}

/*void v_opl_set_coeff_slow(
 * t_opl_one_pole* a_opl,
 * SGFLT a_cutoff //Cutoff in MIDI note number.  Typically 0 to 120
 * )
 *
 * This one is more computationally expensive than the regular
 * function because it doesn't use the approximated midi_note_to_hz function.
 */
void v_opl_set_coeff_slow(t_opl_one_pole* a_opl, SGFLT a_cutoff)
{
    a_opl->cutoff = f_pit_midi_note_to_hz(a_cutoff);
    a_opl->x = exp(-2.0f*PI*((a_opl->cutoff)*(a_opl->sr_recip)));
    a_opl->a0 = 1.0f-(a_opl->x);
    a_opl->b1 = -(a_opl->x);
}

/*void v_opl_set_coeff_hz(
 * t_opl_one_pole* a_opl,
 * SGFLT a_cutoff //Cutoff in MIDI note number.  Typically 0 to 120
 * )
 *
 * This one allows you to set the frequency directly in Hz.
 */
void v_opl_set_coeff_hz(t_opl_one_pole* a_opl, SGFLT a_cutoff)
{
    a_opl->cutoff = a_cutoff;
    a_opl->x = exp(-2.0f*PI*((a_opl->cutoff)*(a_opl->sr_recip)));
    a_opl->a0 = 1.0f-(a_opl->x);
    a_opl->b1 = -(a_opl->x);
}

void v_opl_run(t_opl_one_pole* a_opl, SGFLT a_input)
{
    a_opl->output = f_remove_denormal(((a_opl->a0)*a_input) -
            ((a_opl->b1)*(a_opl->output)));
    a_opl->hp = a_input - (a_opl->output);

#ifdef OPL_DEBUG_MODE
    a_opl->debug_counter = (a_opl->debug_counter) + 1;

    if((a_opl->debug_counter) >= 100000)
    {
        a_opl->debug_counter = 0;

        printf("\n\nOne pole info\n");
        printf("a_opl->a0 == %f\n", a_opl->a0);
        printf("a_opl->a1 == %f\n", a_opl->a1);
        printf("a_opl->b1 == %f\n", a_opl->b1);
        printf("a_opl->cutoff == %f\n", a_opl->cutoff);
        printf("a_opl->hp == %f\n", a_opl->hp);
        printf("a_opl->output == %f\n", a_opl->output);
        printf("a_opl->x == %f\n", a_opl->x);

    }
#endif
}

/*t_opl_one_pole * g_opl_get_one_pole(
 * SGFLT a_sr  //sample rate
 * ) *
 */
t_opl_one_pole * g_opl_get_one_pole(SGFLT a_sr)
{
    t_opl_one_pole * f_result = (t_opl_one_pole*)malloc(sizeof(t_opl_one_pole));

    f_result->a0 = 0.0f;
    f_result->b1 = 0.0f;
    f_result->output = 0.0f;
    f_result->cutoff = 1000.0f;
    f_result->sample_rate = a_sr;
    f_result->sr_recip = 1.0f / a_sr;
#ifdef OPL_DEBUG_MODE
    f_result->debug_counter = 0;
#endif

    return f_result;
}

