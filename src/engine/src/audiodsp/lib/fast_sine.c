#include "audiodsp/constants.h"
#include "audiodsp/lib/fast_sine.h"
#include "audiodsp/lib/interpolate-linear.h"


SG_THREAD_LOCAL SGFLT ARR_SINE[ARR_SINE_COUNT]
 = {
0.000000, 0.007836, 0.015672, 0.023506, 0.031340, 0.039171, 0.047000, 0.054826, 0.062648, 0.070467, 0.078282, 0.086091, 0.093896, 0.101694, 0.109486, 0.117272, 0.125051, 0.132821, 0.140584, 0.148338,
0.156083, 0.163818, 0.171543, 0.179258, 0.186961, 0.194654, 0.202334, 0.210002, 0.217657, 0.225298, 0.232926, 0.240539, 0.248138, 0.255721, 0.263289, 0.270840, 0.278375, 0.285893, 0.293393, 0.300876,
0.308339, 0.315784, 0.323210, 0.330615, 0.338001, 0.345365, 0.352708, 0.360030, 0.367330, 0.374607, 0.381861, 0.389091, 0.396298, 0.403480, 0.410638, 0.417770, 0.424877, 0.431957, 0.439011, 0.446038,
0.453038, 0.460010, 0.466954, 0.473869, 0.480755, 0.487611, 0.494437, 0.501233, 0.507999, 0.514733, 0.521435, 0.528106, 0.534744, 0.541349, 0.547921, 0.554459, 0.560963, 0.567433, 0.573868, 0.580268,
0.586632, 0.592960, 0.599252, 0.605507, 0.611724, 0.617904, 0.624047, 0.630150, 0.636216, 0.642242, 0.648228, 0.654175, 0.660082, 0.665948, 0.671773, 0.677557, 0.683300, 0.689000, 0.694658, 0.700274,
0.705846, 0.711375, 0.716861, 0.722302, 0.727699, 0.733052, 0.738359, 0.743621, 0.748838, 0.754008, 0.759132, 0.764210, 0.769240, 0.774224, 0.779159, 0.784047, 0.788887, 0.793678, 0.798421, 0.803115,
0.807759, 0.812354, 0.816898, 0.821393, 0.825837, 0.830231, 0.834573, 0.838865, 0.843104, 0.847292, 0.851428, 0.855512, 0.859543, 0.863521, 0.867447, 0.871319, 0.875137, 0.878902, 0.882613, 0.886270,
0.889872, 0.893419, 0.896912, 0.900350, 0.903732, 0.907059, 0.910330, 0.913545, 0.916705, 0.919807, 0.922854, 0.925844, 0.928777, 0.931652, 0.934471, 0.937232, 0.939936, 0.942582, 0.945170, 0.947700,
0.950172, 0.952586, 0.954941, 0.957237, 0.959475, 0.961653, 0.963773, 0.965834, 0.967835, 0.969777, 0.971659, 0.973481, 0.975244, 0.976947, 0.978590, 0.980173, 0.981695, 0.983157, 0.984559, 0.985901,
0.987182, 0.988402, 0.989562, 0.990661, 0.991699, 0.992676, 0.993592, 0.994447, 0.995241, 0.995974, 0.996646, 0.997257, 0.997806, 0.998294, 0.998721, 0.999087, 0.999391, 0.999634, 0.999815, 0.999935,
0.999994, 0.999991, 0.999927, 0.999801, 0.999614, 0.999366, 0.999056, 0.998685, 0.998252, 0.997759, 0.997204, 0.996588, 0.995910, 0.995172, 0.994372, 0.993511, 0.992589, 0.991607, 0.990563, 0.989459,
0.988294, 0.987068, 0.985781, 0.984434, 0.983027, 0.981559, 0.980031, 0.978443, 0.976794, 0.975086, 0.973318, 0.971490, 0.969602, 0.967655, 0.965649, 0.963583, 0.961458, 0.959274, 0.957031, 0.954729,
0.952369, 0.949950, 0.947473, 0.944937, 0.942344, 0.939693, 0.936984, 0.934217, 0.931393, 0.928512, 0.925574, 0.922579, 0.919528, 0.916420, 0.913255, 0.910035, 0.906759, 0.903427, 0.900040, 0.896597,
0.893099, 0.889547, 0.885939, 0.882278, 0.878562, 0.874792, 0.870969, 0.867092, 0.863162, 0.859179, 0.855143, 0.851054, 0.846914, 0.842721, 0.838477, 0.834181, 0.829833, 0.825435, 0.820987, 0.816487,
0.811938, 0.807339, 0.802690, 0.797992, 0.793245, 0.788449, 0.783605, 0.778713, 0.773773, 0.768785, 0.763750, 0.758668, 0.753540, 0.748365, 0.743145, 0.737879, 0.732567, 0.727211, 0.721810, 0.716364,
0.710875, 0.705342, 0.699765, 0.694146, 0.688484, 0.682779, 0.677033, 0.671246, 0.665416, 0.659547, 0.653636, 0.647686, 0.641696, 0.635666, 0.629597, 0.623490, 0.617344, 0.611161, 0.604940, 0.598681,
0.592386, 0.586055, 0.579688, 0.573285, 0.566846, 0.560373, 0.553866, 0.547325, 0.540750, 0.534142, 0.527501, 0.520827, 0.514122, 0.507385, 0.500617, 0.493818, 0.486989, 0.480130, 0.473241, 0.466324,
0.459377, 0.452403, 0.445401, 0.438371, 0.431315, 0.424232, 0.417123, 0.409988, 0.402828, 0.395644, 0.388435, 0.381202, 0.373946, 0.366667, 0.359365, 0.352042, 0.344696, 0.337330, 0.329943, 0.322535,
0.315108, 0.307662, 0.300196, 0.292712, 0.285210, 0.277691, 0.270155, 0.262602, 0.255032, 0.247448, 0.239848, 0.232233, 0.224604, 0.216961, 0.209305, 0.201636, 0.193955, 0.186262, 0.178557, 0.170841,
0.163115, 0.155379, 0.147633, 0.139879, 0.132115, 0.124344, 0.116565, 0.108778, 0.100985, 0.093186, 0.085381, 0.077571, 0.069756, 0.061937, 0.054114, 0.046288, 0.038459, 0.030628, 0.022794, 0.014959,
0.007124, -0.000712, -0.008548, -0.016384, -0.024219, -0.032052, -0.039883, -0.047711, -0.055537, -0.063359, -0.071178, -0.078992, -0.086801, -0.094605, -0.102403, -0.110195, -0.117980, -0.125757, -0.133527, -0.141289,
-0.149042, -0.156786, -0.164521, -0.172245, -0.179959, -0.187661, -0.195352, -0.203031, -0.210698, -0.218352, -0.225992, -0.233619, -0.241231, -0.248828, -0.256410, -0.263976, -0.271526, -0.279060, -0.286576, -0.294074,
-0.301555, -0.309017, -0.316460, -0.323884, -0.331287, -0.338671, -0.346034, -0.353375, -0.360695, -0.367992, -0.375267, -0.382519, -0.389747, -0.396952, -0.404132, -0.411287, -0.418417, -0.425521, -0.432600, -0.439651,
-0.446676, -0.453673, -0.460642, -0.467584, -0.474496, -0.481379, -0.488233, -0.495056, -0.501850, -0.508612, -0.515343, -0.522043, -0.528710, -0.535345, -0.541948, -0.548517, -0.555052, -0.561553, -0.568020, -0.574451,
-0.580848, -0.587209, -0.593534, -0.599822, -0.606073, -0.612288, -0.618464, -0.624603, -0.630703, -0.636765, -0.642788, -0.648771, -0.654714, -0.660617, -0.666479, -0.672301, -0.678081, -0.683820, -0.689516, -0.695171,
-0.700782, -0.706351, -0.711876, -0.717357, -0.722795, -0.728188, -0.733536, -0.738840, -0.744097, -0.749310, -0.754476, -0.759596, -0.764669, -0.769695, -0.774674, -0.779606, -0.784489, -0.789325, -0.794112, -0.798850,
-0.803539, -0.808179, -0.812769, -0.817309, -0.821799, -0.826239, -0.830628, -0.834965, -0.839252, -0.843487, -0.847670, -0.851802, -0.855880, -0.859907, -0.863880, -0.867801, -0.871668, -0.875482, -0.879242, -0.882948,
-0.886599, -0.890197, -0.893739, -0.897227, -0.900660, -0.904037, -0.907359, -0.910625, -0.913835, -0.916989, -0.920087, -0.923128, -0.926113, -0.929040, -0.931911, -0.934724, -0.937481, -0.940179, -0.942820, -0.945403,
-0.947927, -0.950394, -0.952802, -0.955152, -0.957443, -0.959675, -0.961849, -0.963963, -0.966018, -0.968014, -0.969950, -0.971827, -0.973644, -0.975401, -0.977099, -0.978736, -0.980313, -0.981831, -0.983287, -0.984684,
-0.986020, -0.987295, -0.988510, -0.989664, -0.990758, -0.991790, -0.992762, -0.993672, -0.994522, -0.995310, -0.996038, -0.996704, -0.997309, -0.997853, -0.998336, -0.998757, -0.999117, -0.999415, -0.999653, -0.999828,
-0.999943, -0.999996, -0.999988, -0.999918, -0.999787, -0.999594, -0.999340, -0.999025, -0.998648, -0.998210, -0.997711, -0.997150, -0.996529, -0.995846, -0.995101, -0.994296, -0.993430, -0.992503, -0.991514, -0.990465,
-0.989355, -0.988185, -0.986953, -0.985661, -0.984309, -0.982896, -0.981423, -0.979889, -0.978295, -0.976642, -0.974928, -0.973154, -0.971321, -0.969428, -0.967475, -0.965463, -0.963392, -0.961262, -0.959072, -0.956824,
-0.954517, -0.952151, -0.949727, -0.947245, -0.944704, -0.942105, -0.939449, -0.936735, -0.933963, -0.931134, -0.928248, -0.925304, -0.922304, -0.919248, -0.916134, -0.912965, -0.909740, -0.906458, -0.903121, -0.899729,
-0.896281, -0.892778, -0.889221, -0.885609, -0.881942, -0.878222, -0.874447, -0.870619, -0.866737, -0.862802, -0.858814, -0.854773, -0.850680, -0.846535, -0.842337, -0.838088, -0.833788, -0.829436, -0.825033, -0.820580,
-0.816076, -0.811522, -0.806918, -0.802265, -0.797562, -0.792811, -0.788011, -0.783162, -0.778266, -0.773321, -0.768329, -0.763290, -0.758204, -0.753071, -0.747893, -0.742668, -0.737398, -0.732082, -0.726721, -0.721316,
-0.715867, -0.710373, -0.704836, -0.699256, -0.693633, -0.687967, -0.682259, -0.676509, -0.670717, -0.664885, -0.659011, -0.653097, -0.647143, -0.641149, -0.635116, -0.629044, -0.622933, -0.616784, -0.610597, -0.604372,
-0.598111, -0.591812, -0.585478, -0.579107, -0.572701, -0.566259, -0.559783, -0.553273, -0.546728, -0.540150, -0.533539, -0.526895, -0.520219, -0.513511, -0.506771, -0.500000, -0.493198, -0.486367, -0.479505, -0.472614,
-0.465693, -0.458744, -0.451768, -0.444763, -0.437731, -0.430672, -0.423586, -0.416475, -0.409338, -0.402176, -0.394989, -0.387778, -0.380543, -0.373285, -0.366004, -0.358700, -0.351375, -0.344028, -0.336659, -0.329270,
-0.321861, -0.314432, -0.306984, -0.299517, -0.292031, -0.284528, -0.277007, -0.269469, -0.261914, -0.254344, -0.246757, -0.239156, -0.231540, -0.223910, -0.216266, -0.208608, -0.200938, -0.193256, -0.185562, -0.177856,
-0.170139, -0.162412, -0.154675, -0.146929, -0.139173, -0.131409, -0.123637, -0.115857, -0.108070, -0.100277, -0.092477, -0.084672, -0.076861, -0.069046, -0.061226, -0.053403, -0.045576, -0.037747, -0.029915, -0.022082,
-0.014247, -0.006411};


/* SGFLT f_sine_fast_run(SGFLT a_osc_core)
 *
 * Accepts zero to one input.
 */
SGFLT f_sine_fast_run(SGFLT a_osc_core)
{
    return f_linear_interpolate_ptr_wrap(ARR_SINE, ARR_SINE_COUNT,
            (a_osc_core * ARR_SINE_COUNT));
}

/* SGFLT f_sine_fast_run(SGFLT a_osc_core)
 *
 * Accepts input in radians (zero to (PI * 2))
 */
SGFLT f_sine_fast_run_radians(SGFLT a_osc_core)
{
    return f_linear_interpolate_ptr_wrap(ARR_SINE, ARR_SINE_COUNT,
            (a_osc_core * ARR_SINE_COUNT_RADIANS));
}

/* SGFLT f_cosine_fast_run(
 * SGFLT a_osc_core,
 * t_lin_interpolater * a_lin)
 *
 * Perform a fast, table-lookup based cosine.  Accepts zero to one input
 */
SGFLT f_cosine_fast_run(SGFLT a_osc_core)
{
    return f_linear_interpolate_ptr_wrap(ARR_SINE, ARR_SINE_COUNT,
            ((a_osc_core * ARR_SINE_COUNT) + 0.25f));
}


/* SGFLT f_cosine_fast_run(
 * SGFLT a_osc_core,
 * t_lin_interpolater * a_lin)
 *
 * Perform a fast, table-lookup based cosine.
 * Accepts input in Radians (zero to (PI * 2))
 */
SGFLT f_cosine_fast_run_radians(SGFLT a_osc_core)
{
    return f_linear_interpolate_ptr_wrap(ARR_SINE, ARR_SINE_COUNT,
        ((a_osc_core * ARR_SINE_COUNT_RADIANS) + ARR_COSINE_PHASE_RADIANS));
}

