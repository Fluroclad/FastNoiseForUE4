// FastNoiseLibrary.h
//
// MIT License
//
// Copyright(c) 2017 Jordan Peck
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// The developer's email is jorzixdan.me2@gzixmail.com (for great email, take
// off every 'zix'.)
//

// VERSION: 0.4.1

// This is a modified version of FastNoise by Jordan Peck (Auburns on Github)
// Modified by Daniel Dickson (Fluroclad)
// It has been modified to be usable in Unreal Engine 4 with Blueprints and C++.

// VERSION UE4: 4.16.2

#include "Game.h"
#include "FastNoiseLibrary.h"
//#include <assert.h>

// Variable declarations
uint8 UFastNoiseLibrary::mPerm[512];
uint8 UFastNoiseLibrary::mPerm12[512];

// Variables
int32 UFastNoiseLibrary::mSeed = 1337;

float UFastNoiseLibrary::mFrequency = 0.01f;

TEnumAsByte<EInterpolation>UFastNoiseLibrary::mInterpolation = Quintic;

TEnumAsByte<ENoiseType>UFastNoiseLibrary::mNoiseType = Simplex;

int32 UFastNoiseLibrary::mOctaves = 3;

float UFastNoiseLibrary::mLacunarity = 2.0f;

float UFastNoiseLibrary::mGain = 0.5f;

TEnumAsByte<EFractalType> UFastNoiseLibrary::mFractalType = FBM;

float UFastNoiseLibrary::mFractalBounding;

TEnumAsByte<ECellularDistanceFunction> UFastNoiseLibrary::mCellularDistanceFunction = Euclidean;

TEnumAsByte<ECellularReturnType> UFastNoiseLibrary::mCellularReturnType = CellValue;

UFastNoiseLibrary* UFastNoiseLibrary::mCellularNoiseLookup = nullptr;

float UFastNoiseLibrary::mGradientPerturbAmp = 1 / 0.45f;

const float GRAD_X[] =
{
	1, -1, 1, -1,
	1, -1, 1, -1,
	0, 0, 0, 0
};
const float GRAD_Y[] =
{
	1, 1, -1, -1,
	0, 0, 0, 0,
	1, -1, 1, -1
};
const float GRAD_Z[] =
{
	0, 0, 0, 0,
	1, 1, -1, -1,
	1, 1, -1, -1
};

const float GRAD_4D[] =
{
	0,1,1,1,0,1,1,-1,0,1,-1,1,0,1,-1,-1,
	0,-1,1,1,0,-1,1,-1,0,-1,-1,1,0,-1,-1,-1,
	1,0,1,1,1,0,1,-1,1,0,-1,1,1,0,-1,-1,
	-1,0,1,1,-1,0,1,-1,-1,0,-1,1,-1,0,-1,-1,
	1,1,0,1,1,1,0,-1,1,-1,0,1,1,-1,0,-1,
	-1,1,0,1,-1,1,0,-1,-1,-1,0,1,-1,-1,0,-1,
	1,1,1,0,1,1,-1,0,1,-1,1,0,1,-1,-1,0,
	-1,1,1,0,-1,1,-1,0,-1,-1,1,0,-1,-1,-1,0
};

const float VAL_LUT[] =
{
	0.3490196078, 0.4352941176, -0.4509803922, 0.6392156863, 0.5843137255, -0.1215686275, 0.7176470588, -0.1058823529, 0.3960784314, 0.0431372549, -0.03529411765, 0.3176470588, 0.7254901961, 0.137254902, 0.8588235294, -0.8196078431,
	-0.7960784314, -0.3333333333, -0.6705882353, -0.3882352941, 0.262745098, 0.3254901961, -0.6470588235, -0.9215686275, -0.5294117647, 0.5294117647, -0.4666666667, 0.8117647059, 0.3803921569, 0.662745098, 0.03529411765, -0.6156862745,
	-0.01960784314, -0.3568627451, -0.09019607843, 0.7490196078, 0.8352941176, -0.4039215686, -0.7490196078, 0.9529411765, -0.0431372549, -0.9294117647, -0.6549019608, 0.9215686275, -0.06666666667, -0.4431372549, 0.4117647059, -0.4196078431,
	-0.7176470588, -0.8117647059, -0.2549019608, 0.4901960784, 0.9137254902, 0.7882352941, -1.0, -0.4745098039, 0.7960784314, 0.8509803922, -0.6784313725, 0.4588235294, 1.0, -0.1843137255, 0.4509803922, 0.1450980392,
	-0.231372549, -0.968627451, -0.8588235294, 0.4274509804, 0.003921568627, -0.003921568627, 0.2156862745, 0.5058823529, 0.7647058824, 0.2078431373, -0.5921568627, 0.5764705882, -0.1921568627, -0.937254902, 0.08235294118, -0.08235294118,
	0.9058823529, 0.8274509804, 0.02745098039, -0.168627451, -0.7803921569, 0.1137254902, -0.9450980392, 0.2, 0.01960784314, 0.5607843137, 0.2705882353, 0.4431372549, -0.9607843137, 0.6156862745, 0.9294117647, -0.07450980392,
	0.3098039216, 0.9921568627, -0.9137254902, -0.2941176471, -0.3411764706, -0.6235294118, -0.7647058824, -0.8901960784, 0.05882352941, 0.2392156863, 0.7333333333, 0.6549019608, 0.2470588235, 0.231372549, -0.3960784314, -0.05098039216,
	-0.2235294118, -0.3725490196, 0.6235294118, 0.7019607843, -0.8274509804, 0.4196078431, 0.07450980392, 0.8666666667, -0.537254902, -0.5058823529, -0.8039215686, 0.09019607843, -0.4823529412, 0.6705882353, -0.7882352941, 0.09803921569,
	-0.6078431373, 0.8039215686, -0.6, -0.3254901961, -0.4117647059, -0.01176470588, 0.4823529412, 0.168627451, 0.8745098039, -0.3647058824, -0.1607843137, 0.568627451, -0.9921568627, 0.9450980392, 0.5137254902, 0.01176470588,
	-0.1450980392, -0.5529411765, -0.5764705882, -0.1137254902, 0.5215686275, 0.1607843137, 0.3725490196, -0.2, -0.7254901961, 0.631372549, 0.7098039216, -0.568627451, 0.1294117647, -0.3098039216, 0.7411764706, -0.8509803922,
	0.2549019608, -0.6392156863, -0.5607843137, -0.3176470588, 0.937254902, 0.9843137255, 0.5921568627, 0.6941176471, 0.2862745098, -0.5215686275, 0.1764705882, 0.537254902, -0.4901960784, -0.4588235294, -0.2078431373, -0.2156862745,
	0.7725490196, 0.3647058824, -0.2392156863, 0.2784313725, -0.8823529412, 0.8980392157, 0.1215686275, 0.1058823529, -0.8745098039, -0.9843137255, -0.7019607843, 0.9607843137, 0.2941176471, 0.3411764706, 0.1529411765, 0.06666666667,
	-0.9764705882, 0.3019607843, 0.6470588235, -0.5843137255, 0.05098039216, -0.5137254902, -0.137254902, 0.3882352941, -0.262745098, -0.3019607843, -0.1764705882, -0.7568627451, 0.1843137255, -0.5450980392, -0.4980392157, -0.2784313725,
	-0.9529411765, -0.09803921569, 0.8901960784, -0.2862745098, -0.3803921569, 0.5529411765, 0.7803921569, -0.8352941176, 0.6862745098, 0.7568627451, 0.4980392157, -0.6862745098, -0.8980392157, -0.7725490196, -0.7098039216, -0.2470588235,
	-0.9058823529, 0.9764705882, 0.1921568627, 0.8431372549, -0.05882352941, 0.3568627451, 0.6078431373, 0.5450980392, 0.4039215686, -0.7333333333, -0.4274509804, 0.6, 0.6784313725, -0.631372549, -0.02745098039, -0.1294117647,
	0.3333333333, -0.8431372549, 0.2235294118, -0.3490196078, -0.6941176471, 0.8823529412, 0.4745098039, 0.4666666667, -0.7411764706, -0.2705882353, 0.968627451, 0.8196078431, -0.662745098, -0.4352941176, -0.8666666667, -0.1529411765,
};

const float CELL_2D_X[] =
{
	-0.4313539279, -0.1733316799, -0.2821957395, -0.2806473808, 0.3125508975, 0.3383018443, -0.4393982022, -0.4460443703, -0.302223039, -0.212681052, -0.2991156529, 0.2293323691, 0.4475439151, 0.1777518, 0.1688522499, -0.0976597166,
	0.08450188373, -0.4098760448, 0.3476585782, -0.3350670039, 0.2298190031, -0.01069924099, -0.4460141246, 0.3650293864, -0.349479423, -0.4122720642, -0.267327811, 0.322124041, 0.2880445931, 0.3892170926, 0.4492085018, -0.4497724772,
	0.1278175387, -0.03572100503, -0.4297407068, -0.3217817723, -0.3057158873, -0.414503978, -0.3738139881, 0.2236891408, 0.002967775577, 0.1747128327, -0.4423772489, -0.2763960987, -0.4019385906, 0.3871414161, -0.430008727, -0.03037574274,
	-0.3486181573, 0.04553517144, -0.0375802926, 0.3266408905, 0.06540017593, 0.03409025829, -0.4449193635, -0.4255936157, 0.449917292, 0.05242606404, -0.4495305179, -0.1204775703, -0.341986385, 0.3865320182, 0.04506097811, -0.06283465979,
	0.3932600341, 0.4472261803, 0.3753571011, -0.273662295, 0.1700461538, 0.4102692229, 0.323227187, -0.2882310238, 0.2050972664, 0.4414085979, -0.1684700334, -0.003978032396, -0.2055133639, -0.006095674897, -0.1196228124, 0.3901528491,
	0.01723531752, -0.3015070339, -0.01514262423, -0.4142574071, -0.1916377265, 0.3749248747, -0.2237774255, -0.4166343106, 0.3619171625, 0.1891126846, -0.3127425077, -0.3281807787, -0.2294806661, -0.3445266136, -0.4167095422, -0.257890321,
	-0.3612037825, 0.2267996491, 0.207157062, 0.08355176718, -0.4312233307, 0.3257055497, 0.177701095, -0.445182522, 0.3955143435, -0.4264613988, -0.3793799665, 0.04617599081, -0.371405428, 0.2563570295, 0.03476646309, -0.3065454405,
	-0.2256979823, 0.4116448463, -0.2907745828, 0.2842278468, 0.3114589359, 0.4464155859, -0.3037334033, 0.4079607166, -0.3486948919, 0.3264821436, 0.3211142406, 0.01183382662, 0.4333844092, 0.3118668416, -0.272753471, -0.422228622,
	-0.1009700099, -0.2741171231, -0.1465125133, 0.2302279044, -0.3699435608, 0.105700352, -0.2646713633, 0.3521828122, -0.1864187807, 0.1994492955, 0.3937065066, -0.3226158377, 0.3796235338, 0.1482921929, -0.407400394, 0.4212853031,
	-0.2621297173, -0.2536986953, -0.2100236383, 0.3624152444, -0.3645038479, 0.2318486784, -0.3260457004, -0.2130045332, 0.3814998766, -0.342977305, -0.4355865605, -0.2104679605, 0.3348364681, 0.3430468811, -0.2291836801, 0.2547707298,
	0.4236174945, -0.15387742, -0.4407449312, -0.06805276192, 0.4453517192, 0.2562464609, 0.3278198355, -0.4122774207, 0.3354090914, 0.446632869, -0.1608953296, -0.09463954939, -0.02637688324, 0.447102804, -0.4365670908, -0.3959858651,
	-0.4240048207, -0.3882794568, -0.4283652566, 0.3303888091, 0.3321434919, -0.413021046, 0.08403060337, -0.3822882919, -0.3712395594, 0.4472363971, -0.4466591209, 0.0486272539, -0.4203101295, 0.2205360833, -0.3624900666, -0.4036086833,
	0.2152727807, -0.4359392962, 0.4178354266, 0.2007630161, -0.07278067175, 0.3644748615, -0.4317451775, -0.297436456, -0.2998672222, -0.2673674124, 0.2808423357, 0.3498946567, -0.2229685561, 0.3305823267, -0.2436681211, -0.03402776529,
	-0.319358823, 0.4454633477, 0.4483504221, -0.4427358436, 0.05452298565, -0.2812560807, 0.1266696921, -0.3735981243, 0.2959708351, -0.3714377181, -0.404467102, 0.1636165687, 0.3289185495, -0.2494824991, 0.03283133272, -0.166306057,
	-0.106833179, 0.06440260376, -0.4483230967, -0.421377757, 0.05097920662, 0.2050584153, 0.4178098529, -0.3565189504, 0.4478398129, -0.3399999602, 0.3767121994, -0.3138934434, -0.1462001792, 0.3970290489, 0.4459149305, -0.4104889426,
	0.1475103971, 0.09258030352, -0.1589664637, 0.2482445008, 0.4383624232, 0.06242802956, 0.2846591015, -0.344202744, 0.1198188883, -0.243590703, 0.2958191174, -0.1164007991, 0.1274037151, 0.368047306, 0.2451436949, -0.4314509715,
};
const float CELL_2D_Y[] =
{
	0.1281943404, 0.415278375, -0.3505218461, 0.3517627718, -0.3237467165, -0.2967353402, -0.09710417025, -0.05953502905, 0.3334085102, -0.3965687458, 0.3361990872, 0.3871778202, -0.04695150755, 0.41340573, -0.4171197882, 0.4392750616,
	0.4419948321, -0.1857461384, -0.2857157906, -0.30038326, -0.3868891648, 0.449872789, -0.05976119672, 0.2631606867, 0.2834856838, 0.1803655873, 0.3619887311, -0.3142230135, -0.3457315612, -0.2258540565, -0.02667811596, 0.01430799601,
	-0.4314657307, 0.4485799926, -0.1335025276, 0.3145735065, 0.3302087162, 0.1751754899, 0.2505256519, -0.3904653228, -0.4499902136, -0.4146991995, -0.08247647938, -0.355112935, -0.2023496216, -0.2293938184, 0.1326367019, -0.4489736231,
	0.2845441624, -0.4476902368, 0.4484280562, 0.3095250049, -0.4452222108, 0.448706869, 0.06742966669, -0.1461850686, 0.008627302568, 0.4469356864, -0.02055026661, 0.4335725488, -0.2924813028, 0.2304191809, -0.447738214, 0.4455915232,
	-0.2187385324, -0.04988730975, -0.2482076684, 0.357223947, 0.4166344988, 0.1848760794, -0.3130881435, -0.3455761521, 0.4005435199, -0.08751256895, 0.4172743077, 0.4499824166, 0.4003301853, -0.4499587123, -0.4338091548, -0.2242337048,
	0.4496698165, 0.3340561458, -0.4497451511, -0.1757577897, -0.4071547394, 0.2488600778, 0.3904147331, -0.1700466149, 0.267424695, -0.4083336779, 0.323561623, 0.307891826, 0.3870899429, 0.2894847362, -0.1698621719, -0.3687717212,
	0.2683874578, 0.3886668486, 0.3994821043, -0.4421754202, 0.1286329626, 0.3105090899, -0.4134275279, 0.06566979625, 0.2146355146, 0.1436338239, -0.2420141339, -0.4476245948, -0.2540826796, -0.3698392535, 0.4486549822, 0.3294387544,
	0.3893076172, -0.1817925206, -0.3434387019, -0.348876097, -0.3247973695, -0.0566844308, -0.3320331606, 0.1899159123, -0.2844501228, 0.3096924441, 0.3152548881, 0.4498443737, 0.1211526057, 0.324405723, 0.3579183483, -0.1556373694,
	-0.4385260051, -0.3568750521, 0.4254810025, -0.3866459777, 0.2562064828, -0.4374099171, 0.3639355292, 0.2801200935, -0.4095705534, -0.4033856449, 0.2179339044, 0.3137180602, 0.2416318948, 0.4248640083, 0.1911149365, 0.1581729856,
	0.3657704353, -0.3716678248, 0.3979825013, 0.2667493029, -0.2638881295, 0.3856762766, 0.3101519002, -0.3963950918, -0.2386584257, 0.2913186713, 0.1129794154, 0.3977477059, -0.3006402163, 0.2912367377, -0.3872658529, -0.3709337882,
	-0.151816397, 0.4228731957, 0.09079595574, -0.444824484, -0.06451237284, -0.3699158705, -0.3082761026, -0.1803533432, -0.3000012356, -0.05494615882, 0.4202531296, 0.4399356268, -0.4492262904, -0.05098119915, 0.1091291678, 0.2137643437,
	-0.1507312575, 0.2274622243, -0.1378521198, 0.305521251, -0.3036127481, -0.1786438231, -0.4420846725, 0.2373934748, -0.2543249683, -0.04979563372, 0.05473234629, -0.4473649407, -0.1607463688, 0.39225481, 0.2666476169, -0.1989975647,
	0.3951678503, -0.1116106179, 0.1670735057, 0.4027334247, -0.4440754146, -0.2639281632, 0.126870413, 0.3376855855, 0.3355289094, 0.3619594822, 0.3516071423, 0.2829730186, 0.390877248, 0.3053118493, -0.3783197679, 0.4487116125,
	0.3170330301, -0.06373700535, 0.03849544189, -0.08052932871, 0.4466847255, 0.3512762688, 0.4318041097, 0.2508474468, -0.3389708908, 0.254035473, -0.1972469604, -0.419201167, -0.3071035458, -0.3745109914, 0.4488007393, -0.4181414777,
	0.4371346153, -0.4453676062, 0.03881238203, -0.1579265206, -0.4471030312, -0.4005634111, -0.167137449, -0.2745801121, 0.04403977727, -0.2947881053, 0.2461461331, 0.3224451987, -0.4255884251, -0.2118205239, -0.06049689889, -0.1843877112,
	-0.4251360756, 0.4403735771, -0.4209865359, 0.3753327428, -0.1016778537, 0.4456486745, -0.3485243118, -0.2898697484, -0.4337550392, 0.3783696201, -0.3391033025, 0.4346847754, -0.4315881062, 0.2589231171, 0.3773652989, 0.12786735,
};
const float CELL_3D_X[] =
{
	0.1453787434, -0.01242829687, 0.2877979582, -0.07732986802, 0.1107205875, 0.2755209141, 0.294168941, 0.4000921098, -0.1697304074, -0.1483224484, 0.2623596946, -0.2709003183, -0.03516550699, -0.1267712655, 0.02952021915, -0.2806854217,
	-0.171159547, 0.2113227183, -0.1024352839, -0.3304249877, 0.2091111325, 0.344678154, 0.1984478035, -0.2929008603, -0.1617332831, -0.3582060271, -0.1852067326, 0.3046301062, -0.03816768434, -0.4084952196, -0.02687443361, -0.03801098351,
	0.2371120802, 0.4447660503, 0.01985147278, 0.4274339143, -0.2072988631, -0.3791240978, -0.2098721267, 0.01582798878, -0.1888129464, 0.1612988974, -0.08974491322, 0.07041229526, -0.1082925611, 0.2474100658, -0.1068836661, 0.2396452163,
	-0.3063886072, 0.1593342891, 0.2709690528, -0.1519780427, 0.1699773681, -0.1986155616, -0.1887482106, 0.2659103394, -0.08838976154, -0.04201869311, -0.3230334656, 0.2612720941, 0.385713046, 0.07654967953, 0.4317038818, -0.2890436293,
	-0.2201947582, 0.4161322773, 0.2204718095, -0.1040307469, -0.1432122615, 0.3978380468, -0.2599274663, 0.4032618332, -0.08953470255, 0.118937202, 0.02167047076, -0.3411343612, 0.3162964612, 0.2355138889, -0.02874541518, -0.2461455173,
	0.04208029445, 0.2727458746, -0.1347522818, 0.3829624424, -0.3547613644, 0.2305790207, -0.08323845599, 0.2993663085, -0.2154865723, 0.01683355354, 0.05240429123, 0.00940104872, 0.3465688735, -0.3706867948, 0.2741169781, 0.06413433865,
	-0.388187972, 0.06419469312, -0.1986120739, -0.203203009, -0.1389736354, -0.06555641638, -0.2529246486, 0.1444476522, -0.3643780054, 0.4286142488, 0.165872923, 0.2219610524, 0.04322940318, -0.08481269795, 0.1822082075, -0.3269323334,
	-0.4080485344, 0.2676025294, 0.3024892441, 0.1448494052, 0.4198402157, -0.3008872161, 0.3639310428, 0.3295806598, 0.2776259487, 0.4149000507, 0.145016715, 0.09299023471, 0.1028907093, 0.2683057049, -0.4227307273, -0.1781224702,
	0.4390788626, 0.2972583585, -0.1707002821, 0.3806686614, -0.1751445661, -0.2227237566, 0.1369633021, -0.3529503428, -0.2590744185, -0.3784019401, -0.05635805671, 0.3251428613, -0.4190995804, -0.3253150961, 0.2857945863, -0.2733604046,
	0.219003657, 0.3182767252, -0.03222023115, -0.3087780231, -0.06487611647, 0.3921171432, -0.1606404506, -0.03767771199, 0.1394866832, -0.4345093872, -0.1044637494, 0.2658727501, 0.2051461999, -0.266085566, 0.07849405464, -0.2160686338,
	-0.185779186, 0.02492421743, -0.120167831, -0.02160084693, 0.2597670064, -0.1611553854, -0.3278896792, 0.2822734956, 0.03169341113, 0.2202613604, 0.2933396046, -0.3194922995, -0.3441586045, 0.2703645948, 0.2298568861, 0.09326603877,
	-0.1116165319, 0.2172907365, 0.1991339479, -0.0541918155, 0.08871336998, 0.2787673278, -0.322166438, -0.4277366384, 0.240131882, 0.1448607981, -0.3837065682, -0.4382627882, -0.37728353, 0.1259579313, -0.1406285511, -0.1580694418,
	0.2477612106, 0.2916132853, 0.07365265219, -0.26126526, -0.3721862032, -0.3691191571, 0.2278441737, 0.363398169, -0.304231482, -0.3199312232, 0.2874852279, -0.1451096801, 0.3220090754, -0.1247400865, -0.2829555867, 0.1069384374,
	-0.1420661144, -0.250548338, 0.3265787872, 0.07646097258, 0.3451771584, 0.298137964, 0.2812250376, 0.4390345476, 0.2148373234, 0.2595421179, 0.3182823114, -0.4089859285, -0.2826749061, 0.3483864637, -0.3226415069, 0.4330734858,
	-0.08717822568, -0.2149678299, -0.2687330705, 0.2105665099, 0.4361845915, 0.05333333359, -0.05986216652, 0.3664988455, -0.2341015558, -0.04730947785, -0.2391566239, -0.1242081035, 0.2614832715, -0.2728794681, 0.007892900508, -0.01730330376,
	0.2054835762, -0.3231994983, -0.2669545963, -0.05554372779, -0.2083935713, 0.06989323478, 0.3847566193, -0.3026215288, 0.3450735512, 0.1814473292, -0.03855010448, 0.3533670318, -0.007945601311, 0.4063099273, -0.2016773589, -0.07527055435,
};
const float CELL_3D_Y[] =
{
	-0.4149781685, -0.1457918398, -0.02606483451, 0.2377094325, -0.3552302079, 0.2640521179, 0.1526064594, -0.2034056362, 0.3970864695, -0.3859694688, -0.2354852944, 0.3505271138, 0.3885234328, 0.1920044036, 0.4409685861, -0.266996757,
	0.2141185563, 0.3902405947, 0.2128044156, -0.1566986703, 0.3133278055, -0.1944240454, -0.3214342325, 0.2262915116, 0.006314769776, -0.148303178, -0.3454119342, 0.1026310383, -0.2551766358, 0.1805950793, -0.2749741471, 0.3277859044,
	0.2900386767, 0.03946930643, -0.01503183293, 0.03345994256, 0.2871414597, 0.1281177671, -0.1007087278, 0.4263894424, -0.3160996813, -0.1974805082, 0.229148752, 0.4150230285, -0.1586061639, -0.3309414609, -0.2701644537, 0.06803600538,
	0.2597428179, -0.3114350249, 0.1412648683, 0.3623355133, 0.3456012883, 0.3836276443, -0.2050154888, 0.3015631259, -0.4288819642, 0.3099592485, 0.201549922, 0.2759854499, 0.2193460345, 0.3721732183, -0.02577753072, -0.3418179959,
	0.383023377, -0.1669634289, 0.02654238946, 0.3890079625, 0.371614387, -0.06206669342, 0.2616724959, -0.1124593585, -0.3048244735, -0.2875221847, -0.03284630549, 0.2500031105, 0.3082064153, -0.3439334267, -0.3955933019, 0.02020282325,
	-0.4470439576, 0.2288471896, -0.02720848277, 0.1231931484, 0.1271702173, 0.3063895591, -0.1922245118, -0.2619918095, 0.2706747713, -0.2680655787, 0.4335128183, -0.4472890582, 0.01141914583, -0.2551104378, 0.2139972417, 0.1708718512,
	-0.03973280434, -0.2803682491, -0.3391173584, -0.3871641506, -0.2775901578, 0.342253257, -0.2904227915, 0.1069184044, -0.2447099973, -0.1358496089, -0.3136808464, -0.3658139958, -0.3832730794, -0.4404869674, -0.3953259299, 0.3036542563,
	0.04227858267, -0.01299671652, -0.1009990293, 0.425921681, 0.08062320474, -0.333040905, -0.1291284382, 0.0184175994, -0.2974929052, -0.144793182, -0.0398992945, -0.299732164, -0.361266869, -0.07076041213, -0.07933161816, 0.1806857196,
	-0.02841848598, 0.2382799621, 0.2215845691, 0.1471852559, -0.274887877, -0.2316778837, 0.1341343041, -0.2472893463, -0.2985577559, 0.2199816631, 0.1485737441, 0.09666046873, 0.1406751354, -0.3080335042, -0.05796152095, 0.1973770973,
	0.2410037886, -0.271342949, -0.3331161506, 0.1992794134, -0.4311322747, -0.06294284106, -0.358928121, -0.2290351443, -0.3602213994, 0.005751117145, 0.4168128432, 0.2551943237, 0.1975390727, 0.23483312, -0.3300346342, 0.05376451292,
	0.2148499206, -0.3229954284, 0.4017266681, -0.06885389554, 0.3096300784, -0.09823036005, 0.1461670309, 0.03754421121, 0.347405252, -0.3460788041, 0.3031973659, 0.2453752201, -0.1698856132, -0.3574277231, 0.3744156221, -0.3170108894,
	-0.2985018719, -0.3460005203, 0.3820341668, -0.2103145071, 0.2012117383, 0.3505404674, 0.3067213525, 0.132066775, -0.1612516055, -0.2387819045, -0.2206398454, -0.09082753406, 0.05445141085, 0.348394558, -0.270877371, 0.4162931958,
	-0.2927867412, 0.3312535401, -0.1666159848, -0.2422237692, 0.252790166, -0.255281188, -0.3358364886, -0.2310190248, -0.2698452035, 0.316332536, 0.1642275508, 0.3277541114, 0.0511344108, -0.04333605335, -0.3056190617, 0.3491024667,
	-0.3055376754, 0.3156466809, 0.1871229129, -0.3026690852, 0.2757120714, 0.2852657134, 0.3466716415, -0.09790429955, 0.1850172527, -0.07946825393, -0.307355516, -0.04647718411, 0.07417482322, 0.225442246, -0.1420585388, -0.118868561,
	-0.3909896417, 0.3939973956, 0.322686276, -0.1961317136, -0.1105517485, -0.313639498, 0.1361029153, 0.2550543014, -0.182405731, -0.4222150243, -0.2577696514, 0.4256953395, -0.3650179274, -0.3499628774, -0.1672771315, 0.2978486637,
	-0.3252600376, 0.1564282844, 0.2599343665, 0.3170813944, -0.310922837, -0.3156141536, -0.1605309138, -0.3001537679, 0.08611519592, -0.2788782453, 0.09795110726, 0.2665752752, 0.140359426, -0.1491768253, 0.008816271194, -0.425643481,
};
const float CELL_3D_Z[] =
{
	-0.0956981749, -0.4255470325, -0.3449535616, 0.3741848704, -0.2530858567, -0.238463215, 0.3044271714, 0.03244149937, -0.1265461359, 0.1775613147, 0.2796677792, -0.07901746678, 0.2243054374, 0.3867342179, 0.08470692262, 0.2289725438,
	0.3568720405, -0.07453178509, -0.3830421561, 0.2622305365, -0.2461670583, -0.2142341261, -0.2445373252, 0.2559320961, -0.4198838754, -0.2284613961, -0.2211087107, 0.314908508, -0.3686842991, 0.05492788837, 0.3551999201, 0.3059600725,
	-0.2493099024, 0.05590469027, -0.4493105419, -0.1366772882, -0.2776273824, 0.2057929936, -0.3851122467, 0.1429738373, -0.2587096108, -0.3707885038, -0.3767448739, -0.1590534329, 0.4069604477, 0.1782302128, -0.3436379634, -0.3747549496,
	0.2028785103, -0.2830561951, -0.3303331794, 0.2193527988, 0.2327390037, -0.1260225743, -0.353330953, -0.2021172246, -0.1036702021, 0.3235115047, -0.2398478873, -0.2409749453, 0.07491837764, 0.241095919, 0.1243675091, -0.04598084447,
	-0.08548310451, -0.03817251927, -0.391391981, -0.2008741118, -0.2095065525, 0.2009293758, -0.2578084893, 0.1650235939, 0.3186935478, 0.325092195, -0.4482761547, 0.1537068389, -0.08640228117, -0.1695376245, 0.2125550295, -0.3761704803,
	0.02968078139, -0.2752065618, -0.4284874806, -0.2016512234, 0.2459107769, 0.2354968222, 0.3982726409, -0.2103333191, 0.287751117, -0.3610505186, -0.1087217856, 0.04841609928, -0.2868093776, 0.003156692623, -0.2855959784, 0.4113266307,
	-0.2241236325, 0.3460819069, 0.2192091725, 0.1063600375, -0.3257760473, -0.2847192729, 0.2327739768, 0.4125570634, -0.09922543227, -0.01829506817, -0.2767498872, 0.1393320198, 0.2318037215, -0.03574965489, 0.1140946023, 0.05838957105,
	-0.184956522, 0.36155217, -0.3174892964, -0.0104580805, 0.1404780841, -0.03241355801, -0.2310412139, -0.3058388149, -0.1921504723, -0.09691688386, 0.4241205002, -0.3225111565, 0.247789732, -0.3542668666, -0.1323073187, -0.3716517945,
	-0.09435116353, -0.2394997452, 0.3525077196, -0.1895464869, 0.3102596268, 0.3149912482, -0.4071228836, -0.129514612, -0.2150435121, -0.1044989934, 0.4210102279, -0.2957006485, -0.08405978803, -0.04225456877, 0.3427271751, -0.2980207554,
	-0.3105713639, 0.1660509868, -0.300824678, -0.2596995338, 0.1114273361, -0.2116183942, -0.2187812825, 0.3855169162, 0.2308332918, 0.1169124335, -0.1336202785, 0.2582393035, 0.3484154868, 0.2766800993, -0.2956616708, -0.3910546287,
	0.3490352499, -0.3123343347, 0.1633259825, 0.4441762538, 0.1978643903, 0.4085091653, 0.2713366126, -0.3484423997, -0.2842624114, -0.1849713341, 0.1565989581, -0.200538455, -0.2349334659, 0.04060059933, 0.0973588921, 0.3054595587,
	0.3177080142, -0.1885958001, -0.1299829458, 0.39412061, 0.3926114802, 0.04370535101, 0.06804996813, 0.04582286686, 0.344723946, 0.3528435224, 0.08116235683, -0.04664855374, 0.2391488697, 0.2554522098, -0.3306796947, -0.06491553533,
	-0.2353514536, 0.08793624968, 0.411478311, 0.2748965434, 0.008634938242, 0.03290232422, 0.1944244981, 0.1306597909, 0.1926830856, -0.008816977938, -0.304764754, -0.2720669462, 0.3101538769, -0.4301882115, -0.1703910946, -0.2630430352,
	-0.2982682484, -0.2002316239, 0.2466400438, 0.324106687, -0.0856480183, 0.179547284, 0.05684409612, -0.01278335452, 0.3494474791, 0.3589187731, -0.08203022006, 0.1818526372, 0.3421885344, -0.1740766085, -0.2796816575, -0.02859407492,
	-0.2050050172, -0.03247898316, -0.1617284888, -0.3459683451, 0.004616608544, -0.3182543336, -0.4247264031, -0.05590974511, 0.3382670703, -0.1483114513, -0.2808182972, -0.07652336246, 0.02980623099, 0.07458404908, 0.4176793787, -0.3368779738,
	-0.2334146693, -0.2712420987, -0.2523278991, -0.3144428146, -0.2497981362, 0.3130537363, -0.1693876312, -0.1443188342, 0.2756962409, -0.3029914042, 0.4375151083, 0.08105160988, -0.4274764309, -0.1231199324, -0.4021797064, -0.1251477955,
};

static int32 FastFloor(float f)
{
	return FMath::FloorToInt(f);
}

static int32 FastRound(float f)
{
	return FMath::RoundToInt(f);
}

static int32 FastAbs(int32 i)
{
	return FMath::Abs(i);
}

static float FastAbs(float f)
{
	return FMath::Abs(f);
}

static float Lerp(float a, float b, float t)
{
	return a + t * (b - a);
}

static float InterpHermiteFunc(float t)
{
	return t*t*(3 - 2 * t);
}

static float InterpQuinticFunc(float t)
{
	return t*t*t*(t*(t * 6 - 15) + 10);
}

static float CubicLerp(float a, float b, float c, float d, float t)
{
	float p = (d - c) - (a - b);
	return t * t * t * p + t * t * ((a - b) - p) + t * (c - a) + b;
}

UFastNoiseLibrary::UFastNoiseLibrary()
{
}

UFastNoiseLibrary::~UFastNoiseLibrary()
{
	delete mCellularNoiseLookup;
}

// Inline misc functions
uint8 UFastNoiseLibrary::Index2D_12(uint8 Offset, int32 x, int32 y)
{
	return mPerm12[(x & 0xff) + mPerm[(y & 0xff) + Offset]];
}

uint8 UFastNoiseLibrary::Index3D_12(uint8 Offset, int32 x, int32 y, int32 z)
{
	return mPerm12[(x & 0xff) + mPerm[(y & 0xff) + mPerm[(z & 0xff) + Offset]]];
}

uint8 UFastNoiseLibrary::Index4D_32(uint8 Offset, int32 x, int32 y, int32 z, int32 w)
{
	return mPerm[(x & 0xff) + mPerm[(y & 0xff) + mPerm[(z & 0xff) + mPerm[(w & 0xff) + Offset]]]] & 31;
}

uint8 UFastNoiseLibrary::Index2D_256(uint8 Offset, int32 x, int32 y)
{
	return mPerm[(x & 0xff) + mPerm[(y & 0xff) + Offset]];
}

uint8 UFastNoiseLibrary::Index3D_256(uint8 Offset, int32 x, int32 y, int32 z)
{
	return mPerm[(x & 0xff) + mPerm[(y & 0xff) + mPerm[(z & 0xff) + Offset]]];
}

uint8 UFastNoiseLibrary::Index4D_256(uint8 Offset, int32 x, int32 y, int32 z, int32 w)
{
	return mPerm[(x & 0xff) + mPerm[(y & 0xff) + mPerm[(z & 0xff) + mPerm[(w & 0xff) + Offset]]]];
}

// Hashing
#define X_PRIME 1619
#define Y_PRIME 31337
#define Z_PRIME 6971
#define W_PRIME 1013

static float ValCoord2D(int32 seed, int32 x, int32 y)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;

	return (n * n * n * 60493) / float(2147483648);
}
static float ValCoord3D(int32 seed, int32 x, int32 y, int32 z)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;
	n ^= Z_PRIME * z;

	return (n * n * n * 60493) / float(2147483648);
}
static float ValCoord4D(int32 seed, int32 x, int32 y, int32 z, int32 w)
{
	int32 n = seed;
	n ^= X_PRIME * x;
	n ^= Y_PRIME * y;
	n ^= Z_PRIME * z;
	n ^= W_PRIME * w;

	return (n * n * n * 60493) / float(2147483648);
}

inline float UFastNoiseLibrary::ValCoord2DFast(uint8 Offset, int32 x, int32 y)
{
	return VAL_LUT[Index2D_256(Offset, x, y)];
}

inline float UFastNoiseLibrary::ValCoord3DFast(uint8 Offset, int32 x, int32 y, int32 z)
{
	return VAL_LUT[Index3D_256(Offset, x, y, z)];
}

inline float UFastNoiseLibrary::GradCoord2D(uint8 Offset, int32 x, int32 y, float xd, float yd)
{
	uint8 LutPos = Index2D_12(Offset, x, y);

	return xd*GRAD_X[LutPos] + yd*GRAD_Y[LutPos];
}

inline float UFastNoiseLibrary::GradCoord3D(uint8 Offset, int32 x, int32 y, int32 z, float xd, float yd, float zd)
{
	uint8 LutPos = Index3D_12(Offset, x, y, z);

	return xd*GRAD_X[LutPos] + yd*GRAD_Y[LutPos] + zd*GRAD_Z[LutPos];
}

inline float UFastNoiseLibrary::GradCoord4D(uint8 Offset, int32 x, int32 y, int32 z, int32 w, float xd, float yd, float zd, float wd)
{
	uint8 LutPos = Index4D_32(Offset, x, y, z, w) << 2;

	return xd*GRAD_4D[LutPos] + yd*GRAD_4D[LutPos + 1] + zd*GRAD_4D[LutPos + 2] + wd*GRAD_4D[LutPos + 3];
}

float UFastNoiseLibrary::GetNoise3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mNoiseType)
	{
	case Value:
		return SingleValue3D(0, x, y, z);

	case ValueFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleValueFractalFBM3D(x, y, z);
		case Billow:
			return SingleValueFractalBillow3D(x, y, z);
		case RigidMulti:
			return SingleValueFractalRigidMulti3D(x, y, z);
		default:
			return 0;
		}

	case Perlin:
		return SinglePerlin3D(0, x, y, z);

	case PerlinFractal:
		switch (mFractalType)
		{
		case FBM:
			return SinglePerlinFractalFBM3D(x, y, z);
		case Billow:
			return SinglePerlinFractalBillow3D(x, y, z);
		case RigidMulti:
			return SinglePerlinFractalRigidMulti3D(x, y, z);
		default:
			return 0;
		}

	case Simplex:
		return SingleSimplex3D(0, x, y, z);

	case SimplexFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleSimplexFractalFBM3D(x, y, z);
		case Billow:
			return SingleSimplexFractalBillow3D(x, y, z);
		case RigidMulti:
			return SingleSimplexFractalRigidMulti3D(x, y, z);
		default:
			return 0;
		}

	case Cellular:
		switch (mCellularReturnType)
		{
		case CellValue:
		case NoiseLookup:
		case Distance:
			return SingleCellular3D(x, y, z);
		default:
			return SingleCellular2Edge3D(x, y, z);
		}

	case WhiteNoise:
		return GetWhiteNoise3D(x, y, z);

	case Cubic:
		return SingleCubic3D(0, x, y, z);

	case CubicFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleCubicFractalFBM3D(x, y, z);
		case Billow:
			return SingleCubicFractalBillow3D(x, y, z);
		case RigidMulti:
			return SingleCubicFractalRigidMulti3D(x, y, z);
		}

	default:
		return 0;
	}
}

float UFastNoiseLibrary::GetNoise2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mNoiseType)
	{
	case Value:
		return SingleValue2D(0, x, y);

	case ValueFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleValueFractalFBM2D(x, y);
		case Billow:
			return SingleValueFractalBillow2D(x, y);
		case RigidMulti:
			return SingleValueFractalRigidMulti2D(x, y);
		}

	case Perlin:
		return SinglePerlin2D(0, x, y);

	case PerlinFractal:
		switch (mFractalType)
		{
		case FBM:
			return SinglePerlinFractalFBM2D(x, y);
		case Billow:
			return SinglePerlinFractalBillow2D(x, y);
		case RigidMulti:
			return SinglePerlinFractalRigidMulti2D(x, y);
		}

	case Simplex:
		return SingleSimplex2D(0, x, y);

	case SimplexFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleSimplexFractalFBM2D(x, y);
		case Billow:
			return SingleSimplexFractalBillow2D(x, y);
		case RigidMulti:
			return SingleSimplexFractalRigidMulti2D(x, y);
		}
	case Cellular:
		switch (mCellularReturnType)
		{
		case CellValue:
		case NoiseLookup:
		case Distance:
			return SingleCellular2D(x, y);
		default:
			return SingleCellular2Edge2D(x, y);
		}

	case WhiteNoise:
		return GetWhiteNoise2D(x, y);

	case Cubic:
		return SingleCubic2D(0, x, y);

	case CubicFractal:
		switch (mFractalType)
		{
		case FBM:
			return SingleCubicFractalFBM2D(x, y);
		case Billow:
			return SingleCubicFractalBillow2D(x, y);
		case RigidMulti:
			return SingleCubicFractalRigidMulti2D(x, y);
		}
	}
	return 0;
}

// General
int32 UFastNoiseLibrary::GetSeed()
{
	return mSeed;
}

void UFastNoiseLibrary::SetSeed(int32 Seed)
{
	TArray<bool> availableSeeds;
	availableSeeds.Init(true, 256);
	mSeed = Seed;
	FMath::RandInit(Seed);

	for (uint16 it = 0; it < 256; ++it)
	{
		uint8 nextNum;
		mPerm[it] = it;
		do
		{
			nextNum = FMath::RandRange(0, 255);
		} while (!availableSeeds[nextNum]);

		int32 k = nextNum + it;
		int32 l = mPerm[it];

		mPerm[it] = mPerm[it + 256] = mPerm[k];
		mPerm[k] = l;
		mPerm12[it] = mPerm[it + 256] = mPerm[it] % 12;
	}
}

void UFastNoiseLibrary::SetFrequency(float Frequency)
{
	mFrequency = Frequency;
}

void UFastNoiseLibrary::SetInterpolation(TEnumAsByte<EInterpolation> Interpolation)
{
	mInterpolation = Interpolation;
}

void UFastNoiseLibrary::SetNoiseType(TEnumAsByte<ENoiseType> NoiseType)
{
	mNoiseType = NoiseType;
}

void UFastNoiseLibrary::SetFractalOctaves(int32 Octaves)
{
	mOctaves = Octaves;
}

void UFastNoiseLibrary::SetFractalLacunarity(float Lacunarity)
{
	mLacunarity = Lacunarity;
}

void UFastNoiseLibrary::SetFractalGain(float Gain)
{
	mGain = Gain;
}

void UFastNoiseLibrary::SetFractalType(TEnumAsByte<EFractalType> FractalType)
{
	mFractalType = FractalType;
}

void UFastNoiseLibrary::SetCellularDistanceFunction(TEnumAsByte<ECellularDistanceFunction> CellularDistanceFunction)
{
	mCellularDistanceFunction = CellularDistanceFunction;
}

void UFastNoiseLibrary::SetCellularReturnType(TEnumAsByte<ECellularReturnType> CellularReturnType)
{
	mCellularReturnType = CellularReturnType;
}

void UFastNoiseLibrary::SetCellularNoiseLookup(UFastNoiseLibrary* Noise)
{
	mCellularNoiseLookup = Noise;
}

void UFastNoiseLibrary::SetGradientPerturbAmp(float GradientPerturbAmp)
{
	mGradientPerturbAmp = GradientPerturbAmp;
}

void UFastNoiseLibrary::CalculateFractalBounding()
{
	float Amp = mGain;
	float AmpFractal = 1;

	for (int32 i = 1; i < mOctaves; i++)
	{
		AmpFractal += Amp;
		Amp *= mGain;
	}

	mFractalBounding = 1 / AmpFractal;
}

// White Noise
float UFastNoiseLibrary::GetWhiteNoise4D(float x, float y, float z, float w)
{
	return ValCoord4D(mSeed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16),
		*reinterpret_cast<int32*>(&z) ^ (*reinterpret_cast<int32*>(&z) >> 16),
		*reinterpret_cast<int32*>(&w) ^ (*reinterpret_cast<int32*>(&w) >> 16));
}

float UFastNoiseLibrary::GetWhiteNoise3D(float x, float y, float z)
{
	return ValCoord3D(mSeed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16),
		*reinterpret_cast<int32*>(&z) ^ (*reinterpret_cast<int32*>(&z) >> 16));
}

float UFastNoiseLibrary::GetWhiteNoise2D(float x, float y)
{
	return ValCoord2D(mSeed,
		*reinterpret_cast<int32*>(&x) ^ (*reinterpret_cast<int32*>(&x) >> 16),
		*reinterpret_cast<int32*>(&y) ^ (*reinterpret_cast<int32*>(&y) >> 16));
}

float UFastNoiseLibrary::GetWhiteNoiseInt4D(int32 x, int32 y, int32 z, int32 w)
{
	return ValCoord4D(mSeed, x, y, z, w);
}

float UFastNoiseLibrary::GetWhiteNoiseInt3D(int32 x, int32 y, int32 z)
{
	return ValCoord3D(mSeed, x, y, z);
}

float UFastNoiseLibrary::GetWhiteNoiseInt2D(int32 x, int32 y)
{
	return ValCoord2D(mSeed, x, y);
}


// Value Noise
float UFastNoiseLibrary::GetValueFractal3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleValueFractalFBM3D(x, y, z);
	case Billow:
		return SingleValueFractalBillow3D(x, y, z);
	case RigidMulti:
		return SingleValueFractalRigidMulti3D(x, y, z);
	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleValueFractalFBM3D(float x, float y, float z)
{
	float Sum = SingleValue3D(mPerm[0], x, y, z);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += SingleValue3D(mPerm[i], x, y, z) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleValueFractalBillow3D(float x, float y, float z)
{
	float Sum = FastAbs(SingleValue3D(mPerm[0], x, y, z)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SingleValue3D(mPerm[i], x, y, z)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleValueFractalRigidMulti3D(float x, float y, float z)
{
	float Sum = 1 - FastAbs(SingleValue3D(mPerm[0], x, y, z));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleValue3D(mPerm[i], x, y, z))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetValue3D(float x, float y, float z)
{
	return SingleValue3D(0, x * mFrequency, y * mFrequency, z * mFrequency);
}

float UFastNoiseLibrary::SingleValue3D(uint8 Offset, float x, float y, float z)
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 z0 = FastFloor(z);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs, ys, zs;
	switch (mInterpolation)
	{
	case Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		zs = z - (float)z0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		zs = InterpHermiteFunc(z - (float)z0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		zs = InterpQuinticFunc(z - (float)z0);
		break;

	default:
		xs = ys = zs = 0; // Silence compiler
		break;
	}

	float xf00 = Lerp(ValCoord3DFast(Offset, x0, y0, z0), ValCoord3DFast(Offset, x1, y0, z0), xs);
	float xf10 = Lerp(ValCoord3DFast(Offset, x0, y1, z0), ValCoord3DFast(Offset, x1, y1, z0), xs);
	float xf01 = Lerp(ValCoord3DFast(Offset, x0, y0, z1), ValCoord3DFast(Offset, x1, y0, z1), xs);
	float xf11 = Lerp(ValCoord3DFast(Offset, x0, y1, z1), ValCoord3DFast(Offset, x1, y1, z1), xs);

	float yf0 = Lerp(xf00, xf10, ys);
	float yf1 = Lerp(xf01, xf11, ys);

	return Lerp(yf0, yf1, zs);
}

float UFastNoiseLibrary::GetValueFractal2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleValueFractalFBM2D(x, y);

	case Billow:
		return SingleValueFractalBillow2D(x, y);

	case RigidMulti:
		return SingleValueFractalRigidMulti2D(x, y);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleValueFractalFBM2D(float x, float y)
{
	float Sum = SingleValue2D(mPerm[0], x, y);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += SingleValue2D(mPerm[i], x, y) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleValueFractalBillow2D(float x, float y)
{
	float Sum = FastAbs(SingleValue2D(mPerm[0], x, y)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		Amp *= mGain;
		Sum += (FastAbs(SingleValue2D(mPerm[i], x, y)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleValueFractalRigidMulti2D(float x, float y)
{
	float Sum = 1 - FastAbs(SingleValue2D(mPerm[0], x, y));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleValue2D(mPerm[i], x, y))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetValue2D(float x, float y)
{
	return SingleValue2D(0, x * mFrequency, y * mFrequency);
}

float UFastNoiseLibrary::SingleValue2D(uint8 Offset, float x, float y)
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs, ys;
	switch (mInterpolation)
	{
	case Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		break;

	default:
		xs = ys = 0; // Silence compiler
		break;
	}

	float xf0 = Lerp(ValCoord2DFast(Offset, x0, y0), ValCoord2DFast(Offset, x1, y0), xs);
	float xf1 = Lerp(ValCoord2DFast(Offset, x0, y1), ValCoord2DFast(Offset, x1, y1), xs);

	return Lerp(xf0, xf1, ys);
}

// Perlin Noise
float UFastNoiseLibrary::GetPerlinFractal3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SinglePerlinFractalFBM3D(x, y, z);

	case Billow:
		return SinglePerlinFractalBillow3D(x, y, z);

	case RigidMulti:
		return SinglePerlinFractalRigidMulti3D(x, y, z);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SinglePerlinFractalFBM3D(float x, float y, float z)
{
	float Sum = SinglePerlin3D(mPerm[0], x, y, z);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += SinglePerlin3D(mPerm[i], x, y, z) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SinglePerlinFractalBillow3D(float x, float y, float z)
{
	float Sum = FastAbs(SinglePerlin3D(mPerm[0], x, y, z)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SinglePerlin3D(mPerm[i], x, y, z)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SinglePerlinFractalRigidMulti3D(float x, float y, float z)
{
	float Sum = 1 - FastAbs(SinglePerlin3D(mPerm[0], x, y, z));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SinglePerlin3D(mPerm[i], x, y, z))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetPerlin3D(float x, float y, float z)
{
	return SinglePerlin3D(0, x * mFrequency, y * mFrequency, z * mFrequency);
}

float UFastNoiseLibrary::SinglePerlin3D(uint8 Offset, float x, float y, float z)
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 z0 = FastFloor(z);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs, ys, zs;
	switch (mInterpolation)
	{
	case Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		zs = z - (float)z0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		zs = InterpHermiteFunc(z - (float)z0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		zs = InterpQuinticFunc(z - (float)z0);
		break;

	default:
		xs = ys = zs = 0; // Silence compiler
		break;
	}

	float xd0 = x - (float)x0;
	float yd0 = y - (float)y0;
	float zd0 = z - (float)z0;
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;
	float zd1 = zd0 - 1;

	float xf00 = Lerp(GradCoord3D(Offset, x0, y0, z0, xd0, yd0, zd0), GradCoord3D(Offset, x1, y0, z0, xd1, yd0, zd0), xs);
	float xf10 = Lerp(GradCoord3D(Offset, x0, y1, z0, xd0, yd1, zd0), GradCoord3D(Offset, x1, y1, z0, xd1, yd1, zd0), xs);
	float xf01 = Lerp(GradCoord3D(Offset, x0, y0, z1, xd0, yd0, zd1), GradCoord3D(Offset, x1, y0, z1, xd1, yd0, zd1), xs);
	float xf11 = Lerp(GradCoord3D(Offset, x0, y1, z1, xd0, yd1, zd1), GradCoord3D(Offset, x1, y1, z1, xd1, yd1, zd1), xs);

	float yf0 = Lerp(xf00, xf10, ys);
	float yf1 = Lerp(xf01, xf11, ys);

	return Lerp(yf0, yf1, zs);
}

float UFastNoiseLibrary::GetPerlinFractal2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SinglePerlinFractalFBM2D(x, y);

	case Billow:
		return SinglePerlinFractalBillow2D(x, y);

	case RigidMulti:
		return SinglePerlinFractalRigidMulti2D(x, y);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SinglePerlinFractalFBM2D(float x, float y)
{
	float Sum = SinglePerlin2D(mPerm[0], x, y);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += SinglePerlin2D(mPerm[i], x, y) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SinglePerlinFractalBillow2D(float x, float y)
{
	float Sum = FastAbs(SinglePerlin2D(mPerm[0], x, y)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SinglePerlin2D(mPerm[i], x, y)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SinglePerlinFractalRigidMulti2D(float x, float y)
{
	float Sum = 1 - FastAbs(SinglePerlin2D(mPerm[0], x, y));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SinglePerlin2D(mPerm[i], x, y))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetPerlin2D(float x, float y)
{
	return SinglePerlin2D(0, x * mFrequency, y * mFrequency);
}

float UFastNoiseLibrary::SinglePerlin2D(uint8 Offset, float x, float y)
{
	int32 x0 = FastFloor(x);
	int32 y0 = FastFloor(y);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs, ys;
	switch (mInterpolation)
	{
	case Linear:
		xs = x - (float)x0;
		ys = y - (float)y0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(x - (float)x0);
		ys = InterpHermiteFunc(y - (float)y0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(x - (float)x0);
		ys = InterpQuinticFunc(y - (float)y0);
		break;

	default:
		xs = ys = 0; // Silence compiler
		break;
	}

	float xd0 = x - (float)x0;
	float yd0 = y - (float)y0;
	float xd1 = xd0 - 1;
	float yd1 = yd0 - 1;

	float xf0 = Lerp(GradCoord2D(Offset, x0, y0, xd0, yd0), GradCoord2D(Offset, x1, y0, xd1, yd0), xs);
	float xf1 = Lerp(GradCoord2D(Offset, x0, y1, xd0, yd1), GradCoord2D(Offset, x1, y1, xd1, yd1), xs);

	return Lerp(xf0, xf1, ys);
}

// Simplex Noise
float UFastNoiseLibrary::GetSimplexFractal3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleSimplexFractalFBM3D(x, y, z);

	case Billow:
		return SingleSimplexFractalBillow3D(x, y, z);

	case RigidMulti:
		return SingleSimplexFractalRigidMulti3D(x, y, z);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleSimplexFractalFBM3D(float x, float y, float z)
{
	float Sum = SingleSimplex3D(mPerm[0], x, y, z);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += SingleSimplex3D(mPerm[i], x, y, z) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleSimplexFractalBillow3D(float x, float y, float z)
{
	float Sum = FastAbs(SingleSimplex3D(mPerm[0], x, y, z)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SingleSimplex3D(mPerm[i], x, y, z)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleSimplexFractalRigidMulti3D(float x, float y, float z)
{
	float Sum = 1 - FastAbs(SingleSimplex3D(mPerm[0], x, y, z));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleSimplex3D(mPerm[i], x, y, z))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetSimplex3D(float x, float y, float z)
{
	return SingleSimplex3D(0, x * mFrequency, y * mFrequency, z * mFrequency);
}

static const float F3 = 1 / float(3);
static const float G3 = 1 / float(6);

float UFastNoiseLibrary::SingleSimplex3D(uint8 Offset, float x, float y, float z)
{
	float t = (x + y + z) * F3;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);
	int32 k = FastFloor(z + t);

	t = (i + j + k) * G3;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;

	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;

	int32 i1, j1, k1;
	int32 i2, j2, k2;

	if (x0 >= y0)
	{
		if (y0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
		else if (x0 >= z0)
		{
			i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1;
		}
		else // x0 < z0
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1;
		}
	}
	else // x0 < y0
	{
		if (y0 < z0)
		{
			i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1;
		}
		else if (x0 < z0)
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1;
		}
		else // x0 >= z0
		{
			i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0;
		}
	}

	float x1 = x0 - i1 + G3;
	float y1 = y0 - j1 + G3;
	float z1 = z0 - k1 + G3;
	float x2 = x0 - i2 + 2 * G3;
	float y2 = y0 - j2 + 2 * G3;
	float z2 = z0 - k2 + 2 * G3;
	float x3 = x0 - 1 + 3 * G3;
	float y3 = y0 - 1 + 3 * G3;
	float z3 = z0 - 1 + 3 * G3;

	float n0, n1, n2, n3;

	t = float(0.6) - x0*x0 - y0*y0 - z0*z0;
	if (t < 0) n0 = 0;
	else
	{
		t *= t;
		n0 = t*t*GradCoord3D(Offset, i, j, k, x0, y0, z0);
	}

	t = float(0.6) - x1*x1 - y1*y1 - z1*z1;
	if (t < 0) n1 = 0;
	else
	{
		t *= t;
		n1 = t*t*GradCoord3D(Offset, i + i1, j + j1, k + k1, x1, y1, z1);
	}

	t = float(0.6) - x2*x2 - y2*y2 - z2*z2;
	if (t < 0) n2 = 0;
	else
	{
		t *= t;
		n2 = t*t*GradCoord3D(Offset, i + i2, j + j2, k + k2, x2, y2, z2);
	}

	t = float(0.6) - x3*x3 - y3*y3 - z3*z3;
	if (t < 0) n3 = 0;
	else
	{
		t *= t;
		n3 = t*t*GradCoord3D(Offset, i + 1, j + 1, k + 1, x3, y3, z3);
	}

	return 32 * (n0 + n1 + n2 + n3);
}

float UFastNoiseLibrary::GetSimplexFractal2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleSimplexFractalFBM2D(x, y);

	case Billow:
		return SingleSimplexFractalBillow2D(x, y);

	case RigidMulti:
		return SingleSimplexFractalRigidMulti2D(x, y);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleSimplexFractalFBM2D(float x, float y)
{
	float Sum = SingleSimplex2D(mPerm[0], x, y);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += SingleSimplex2D(mPerm[i], x, y) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleSimplexFractalBillow2D(float x, float y)
{
	float Sum = FastAbs(SingleSimplex2D(mPerm[0], x, y)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SingleSimplex2D(mPerm[i], x, y)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleSimplexFractalRigidMulti2D(float x, float y)
{
	float Sum = 1 - FastAbs(SingleSimplex2D(mPerm[0], x, y));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleSimplex2D(mPerm[i], x, y))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::SingleSimplexFractalBlend2D(float x, float y)
{
	float Sum = SingleSimplex2D(mPerm[0], x, y);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum *= SingleSimplex2D(mPerm[i], x, y) * Amp + 1;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::GetSimplex2D(float x, float y)
{
	return SingleSimplex2D(0, x * mFrequency, y * mFrequency);
}

static const float F2 = 1 / float(2);
static const float G2 = 1 / float(4);

float UFastNoiseLibrary::SingleSimplex2D(uint8 Offset, float x, float y)
{
	float t = (x + y) * F2;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);

	t = (i + j) * G2;
	float X0 = i - t;
	float Y0 = j - t;

	float x0 = x - X0;
	float y0 = y - Y0;

	int32 i1, j1;
	if (x0 > y0)
	{
		i1 = 1; j1 = 0;
	}
	else
	{
		i1 = 0; j1 = 1;
	}

	float x1 = x0 - (float)i1 + G2;
	float y1 = y0 - (float)j1 + G2;
	float x2 = x0 - 1 + 2 * G2;
	float y2 = y0 - 1 + 2 * G2;

	float n0, n1, n2;

	t = float(0.5) - x0*x0 - y0*y0;
	if (t < 0) n0 = 0;
	else
	{
		t *= t;
		n0 = t * t * GradCoord2D(Offset, i, j, x0, y0);
	}

	t = float(0.5) - x1*x1 - y1*y1;
	if (t < 0) n1 = 0;
	else
	{
		t *= t;
		n1 = t*t*GradCoord2D(Offset, i + i1, j + j1, x1, y1);
	}

	t = float(0.5) - x2*x2 - y2*y2;
	if (t < 0) n2 = 0;
	else
	{
		t *= t;
		n2 = t*t*GradCoord2D(Offset, i + 1, j + 1, x2, y2);
	}

	return  50 * (n0 + n1 + n2);
}

float UFastNoiseLibrary::GetSimplex4D(float x, float y, float z, float w)
{
	return SingleSimplex4D(0, x * mFrequency, y * mFrequency, z * mFrequency, w * mFrequency);
}

static const uint8 SIMPLEX_4D[] =
{
	0,1,2,3,0,1,3,2,0,0,0,0,0,2,3,1,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,0,
	0,2,1,3,0,0,0,0,0,3,1,2,0,3,2,1,0,0,0,0,0,0,0,0,0,0,0,0,1,3,2,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,2,0,3,0,0,0,0,1,3,0,2,0,0,0,0,0,0,0,0,0,0,0,0,2,3,0,1,2,3,1,0,
	1,0,2,3,1,0,3,2,0,0,0,0,0,0,0,0,0,0,0,0,2,0,3,1,0,0,0,0,2,1,3,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	2,0,1,3,0,0,0,0,0,0,0,0,0,0,0,0,3,0,1,2,3,0,2,1,0,0,0,0,3,1,2,0,
	2,1,0,3,0,0,0,0,0,0,0,0,0,0,0,0,3,1,0,2,0,0,0,0,3,2,0,1,3,2,1,0
};

static const float F4 = (sqrt(float(5)) - 1) / 4;
static const float G4 = (5 - sqrt(float(5))) / 2;

float UFastNoiseLibrary::SingleSimplex4D(uint8 Offset, float x, float y, float z, float w)
{
	float n0, n1, n2, n3, n4;
	float t = (x + y + z + w) * F4;
	int32 i = FastFloor(x + t);
	int32 j = FastFloor(y + t);
	int32 k = FastFloor(z + t);
	int32 l = FastFloor(w + t);
	t = (i + j + k + l) * G4;
	float X0 = i - t;
	float Y0 = j - t;
	float Z0 = k - t;
	float W0 = l - t;
	float x0 = x - X0;
	float y0 = y - Y0;
	float z0 = z - Z0;
	float w0 = w - W0;

	int32 c = (x0 > y0) ? 32 : 0;
	c += (x0 > z0) ? 16 : 0;
	c += (y0 > z0) ? 8 : 0;
	c += (x0 > w0) ? 4 : 0;
	c += (y0 > w0) ? 2 : 0;
	c += (z0 > w0) ? 1 : 0;
	c <<= 2;

	int32 i1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 i2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 i3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 j1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 j2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 j3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 k1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 k2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 k3 = SIMPLEX_4D[c++] >= 1 ? 1 : 0;
	int32 l1 = SIMPLEX_4D[c] >= 3 ? 1 : 0;
	int32 l2 = SIMPLEX_4D[c] >= 2 ? 1 : 0;
	int32 l3 = SIMPLEX_4D[c] >= 1 ? 1 : 0;

	float x1 = x0 - i1 + G4;
	float y1 = y0 - j1 + G4;
	float z1 = z0 - k1 + G4;
	float w1 = w0 - l1 + G4;
	float x2 = x0 - i2 + 2 * G4;
	float y2 = y0 - j2 + 2 * G4;
	float z2 = z0 - k2 + 2 * G4;
	float w2 = w0 - l2 + 2 * G4;
	float x3 = x0 - i3 + 3 * G4;
	float y3 = y0 - j3 + 3 * G4;
	float z3 = z0 - k3 + 3 * G4;
	float w3 = w0 - l3 + 3 * G4;
	float x4 = x0 - 1 + 4 * G4;
	float y4 = y0 - 1 + 4 * G4;
	float z4 = z0 - 1 + 4 * G4;
	float w4 = w0 - 1 + 4 * G4;

	t = float(0.6) - x0*x0 - y0*y0 - z0*z0 - w0*w0;
	if (t < 0) n0 = 0;
	else {
		t *= t;
		n0 = t * t * GradCoord4D(Offset, i, j, k, l, x0, y0, z0, w0);
	}
	t = float(0.6) - x1*x1 - y1*y1 - z1*z1 - w1*w1;
	if (t < 0) n1 = 0;
	else {
		t *= t;
		n1 = t * t * GradCoord4D(Offset, i + i1, j + j1, k + k1, l + l1, x1, y1, z1, w1);
	}
	t = float(0.6) - x2*x2 - y2*y2 - z2*z2 - w2*w2;
	if (t < 0) n2 = 0;
	else {
		t *= t;
		n2 = t * t * GradCoord4D(Offset, i + i2, j + j2, k + k2, l + l2, x2, y2, z2, w2);
	}
	t = float(0.6) - x3*x3 - y3*y3 - z3*z3 - w3*w3;
	if (t < 0) n3 = 0;
	else {
		t *= t;
		n3 = t * t * GradCoord4D(Offset, i + i3, j + j3, k + k3, l + l3, x3, y3, z3, w3);
	}
	t = float(0.6) - x4*x4 - y4*y4 - z4*z4 - w4*w4;
	if (t < 0) n4 = 0;
	else {
		t *= t;
		n4 = t * t * GradCoord4D(Offset, i + 1, j + 1, k + 1, l + 1, x4, y4, z4, w4);
	}

	return 27 * (n0 + n1 + n2 + n3 + n4);
}

// Cubic Noise
float UFastNoiseLibrary::GetCubicFractal3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleCubicFractalFBM3D(x, y, z);

	case Billow:
		return SingleCubicFractalBillow3D(x, y, z);

	case RigidMulti:
		return SingleCubicFractalRigidMulti3D(x, y, z);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleCubicFractalFBM3D(float x, float y, float z)
{
	float Sum = SingleCubic3D(mPerm[0], x, y, z);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += SingleCubic3D(mPerm[i], x, y, z) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleCubicFractalBillow3D(float x, float y, float z)
{
	float Sum = FastAbs(SingleCubic3D(mPerm[0], x, y, z)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SingleCubic3D(mPerm[i], x, y, z)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleCubicFractalRigidMulti3D(float x, float y, float z)
{
	float Sum = 1 - FastAbs(SingleCubic3D(mPerm[0], x, y, z));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;
		z *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleCubic3D(mPerm[i], x, y, z))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetCubic3D(float x, float y, float z)
{
	return SingleCubic3D(0, x * mFrequency, y * mFrequency, z * mFrequency);
}

const float CUBIC_3D_BOUNDING = 1 / (1.5*1.5*1.5);

float UFastNoiseLibrary::SingleCubic3D(uint8 Offset, float x, float y, float z)
{
	int32 x1 = FastFloor(x);
	int32 y1 = FastFloor(y);
	int32 z1 = FastFloor(z);

	int32 x0 = x1 - 1;
	int32 y0 = y1 - 1;
	int32 z0 = z1 - 1;
	int32 x2 = x1 + 1;
	int32 y2 = y1 + 1;
	int32 z2 = z1 + 1;
	int32 x3 = x1 + 2;
	int32 y3 = y1 + 2;
	int32 z3 = z1 + 2;

	float xs = x - (float)x1;
	float ys = y - (float)y1;
	float zs = z - (float)z1;

	return CubicLerp(
		CubicLerp(
			CubicLerp(ValCoord3DFast(Offset, x0, y0, z0), ValCoord3DFast(Offset, x1, y0, z0), ValCoord3DFast(Offset, x2, y0, z0), ValCoord3DFast(Offset, x3, y0, z0), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y1, z0), ValCoord3DFast(Offset, x1, y1, z0), ValCoord3DFast(Offset, x2, y1, z0), ValCoord3DFast(Offset, x3, y1, z0), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y2, z0), ValCoord3DFast(Offset, x1, y2, z0), ValCoord3DFast(Offset, x2, y2, z0), ValCoord3DFast(Offset, x3, y2, z0), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y3, z0), ValCoord3DFast(Offset, x1, y3, z0), ValCoord3DFast(Offset, x2, y3, z0), ValCoord3DFast(Offset, x3, y3, z0), xs),
			ys),
		CubicLerp(
			CubicLerp(ValCoord3DFast(Offset, x0, y0, z1), ValCoord3DFast(Offset, x1, y0, z1), ValCoord3DFast(Offset, x2, y0, z1), ValCoord3DFast(Offset, x3, y0, z1), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y1, z1), ValCoord3DFast(Offset, x1, y1, z1), ValCoord3DFast(Offset, x2, y1, z1), ValCoord3DFast(Offset, x3, y1, z1), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y2, z1), ValCoord3DFast(Offset, x1, y2, z1), ValCoord3DFast(Offset, x2, y2, z1), ValCoord3DFast(Offset, x3, y2, z1), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y3, z1), ValCoord3DFast(Offset, x1, y3, z1), ValCoord3DFast(Offset, x2, y3, z1), ValCoord3DFast(Offset, x3, y3, z1), xs),
			ys),
		CubicLerp(
			CubicLerp(ValCoord3DFast(Offset, x0, y0, z2), ValCoord3DFast(Offset, x1, y0, z2), ValCoord3DFast(Offset, x2, y0, z2), ValCoord3DFast(Offset, x3, y0, z2), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y1, z2), ValCoord3DFast(Offset, x1, y1, z2), ValCoord3DFast(Offset, x2, y1, z2), ValCoord3DFast(Offset, x3, y1, z2), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y2, z2), ValCoord3DFast(Offset, x1, y2, z2), ValCoord3DFast(Offset, x2, y2, z2), ValCoord3DFast(Offset, x3, y2, z2), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y3, z2), ValCoord3DFast(Offset, x1, y3, z2), ValCoord3DFast(Offset, x2, y3, z2), ValCoord3DFast(Offset, x3, y3, z2), xs),
			ys),
		CubicLerp(
			CubicLerp(ValCoord3DFast(Offset, x0, y0, z3), ValCoord3DFast(Offset, x1, y0, z3), ValCoord3DFast(Offset, x2, y0, z3), ValCoord3DFast(Offset, x3, y0, z3), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y1, z3), ValCoord3DFast(Offset, x1, y1, z3), ValCoord3DFast(Offset, x2, y1, z3), ValCoord3DFast(Offset, x3, y1, z3), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y2, z3), ValCoord3DFast(Offset, x1, y2, z3), ValCoord3DFast(Offset, x2, y2, z3), ValCoord3DFast(Offset, x3, y2, z3), xs),
			CubicLerp(ValCoord3DFast(Offset, x0, y3, z3), ValCoord3DFast(Offset, x1, y3, z3), ValCoord3DFast(Offset, x2, y3, z3), ValCoord3DFast(Offset, x3, y3, z3), xs),
			ys),
		zs) * CUBIC_3D_BOUNDING;
}


float UFastNoiseLibrary::GetCubicFractal2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mFractalType)
	{
	case FBM:
		return SingleCubicFractalFBM2D(x, y);

	case Billow:
		return SingleCubicFractalBillow2D(x, y);

	case RigidMulti:
		return SingleCubicFractalRigidMulti2D(x, y);

	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleCubicFractalFBM2D(float x, float y)
{
	float Sum = SingleCubic2D(mPerm[0], x, y);
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += SingleCubic2D(mPerm[i], x, y) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleCubicFractalBillow2D(float x, float y)
{
	float Sum = FastAbs(SingleCubic2D(mPerm[0], x, y)) * 2 - 1;
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum += (FastAbs(SingleCubic2D(mPerm[i], x, y)) * 2 - 1) * Amp;
	}

	return Sum * mFractalBounding;
}

float UFastNoiseLibrary::SingleCubicFractalRigidMulti2D(float x, float y)
{
	float Sum = 1 - FastAbs(SingleCubic2D(mPerm[0], x, y));
	float Amp = 1;
	int32 i = 0;

	while (++i < mOctaves)
	{
		x *= mLacunarity;
		y *= mLacunarity;

		Amp *= mGain;
		Sum -= (1 - FastAbs(SingleCubic2D(mPerm[i], x, y))) * Amp;
	}

	return Sum;
}

float UFastNoiseLibrary::GetCubic2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	return SingleCubic2D(0, x, y);
}

const float CUBIC_2D_BOUNDING = 1 / (1.5*1.5);

float UFastNoiseLibrary::SingleCubic2D(uint8 Offset, float x, float y)
{
	int32 x1 = FastFloor(x);
	int32 y1 = FastFloor(y);

	int32 x0 = x1 - 1;
	int32 y0 = y1 - 1;
	int32 x2 = x1 + 1;
	int32 y2 = y1 + 1;
	int32 x3 = x1 + 2;
	int32 y3 = y1 + 2;

	float xs = x - (float)x1;
	float ys = y - (float)y1;

	return CubicLerp(
		CubicLerp(ValCoord2DFast(Offset, x0, y0), ValCoord2DFast(Offset, x1, y0), ValCoord2DFast(Offset, x2, y0), ValCoord2DFast(Offset, x3, y0), xs),
		CubicLerp(ValCoord2DFast(Offset, x0, y1), ValCoord2DFast(Offset, x1, y1), ValCoord2DFast(Offset, x2, y1), ValCoord2DFast(Offset, x3, y1), xs),
		CubicLerp(ValCoord2DFast(Offset, x0, y2), ValCoord2DFast(Offset, x1, y2), ValCoord2DFast(Offset, x2, y2), ValCoord2DFast(Offset, x3, y2), xs),
		CubicLerp(ValCoord2DFast(Offset, x0, y3), ValCoord2DFast(Offset, x1, y3), ValCoord2DFast(Offset, x2, y3), ValCoord2DFast(Offset, x3, y3), xs),
		ys) * CUBIC_2D_BOUNDING;
}

// Cellular Noise
float UFastNoiseLibrary::GetCellular3D(float x, float y, float z)
{
	x *= mFrequency;
	y *= mFrequency;
	z *= mFrequency;

	switch (mCellularReturnType)
	{
	case CellValue:
	case NoiseLookup:
	case Distance:
		return SingleCellular3D(x, y, z);

	default:
		return SingleCellular2Edge3D(x, y, z);
	}
}

float UFastNoiseLibrary::SingleCellular3D(float x, float y, float z)
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);
	int32 zr = FastRound(z);

	float distance = 999999;
	int32 xc, yc, zc;

	xc = yc = zc = 0; // Silence compiler

	switch (mCellularDistanceFunction)
	{
	case Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;

	case Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ);

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;

	case Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = (FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ)) + (vecX * vecX + vecY * vecY + vecZ * vecZ);

					if (newDistance < distance)
					{
						distance = newDistance;
						xc = xi;
						yc = yi;
						zc = zi;
					}
				}
			}
		}
		break;

	default:
		break;
	}

	uint8 lutPos;
	switch (mCellularReturnType)
	{
	case CellValue:
		return ValCoord3D(mSeed, xc, yc, zc);

	case NoiseLookup:
		//assert(mCellularNoiseLookup);

		lutPos = Index3D_256(0, xc, yc, zc);
		return mCellularNoiseLookup->GetNoise3D(xc + CELL_3D_X[lutPos], yc + CELL_3D_Y[lutPos], zc + CELL_3D_Z[lutPos]);

	case Distance:
		return distance - 1;
	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleCellular2Edge3D(float x, float y, float z)
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);
	int32 zr = FastRound(z);

	float distance = 999999;
	float distance2 = 999999;

	switch (mCellularDistanceFunction)
	{
	case Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = vecX * vecX + vecY * vecY + vecZ * vecZ;

					distance2 = fmax(fmin(distance2, newDistance), distance);
					distance = fmin(distance, newDistance);
				}
			}
		}
		break;

	case Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ);

					distance2 = fmax(fmin(distance2, newDistance), distance);
					distance = fmin(distance, newDistance);
				}
			}
		}
		break;

	case Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				for (int32 zi = zr - 1; zi <= zr + 1; zi++)
				{
					uint8 lutPos = Index3D_256(0, xi, yi, zi);

					float vecX = xi - x + CELL_3D_X[lutPos];
					float vecY = yi - y + CELL_3D_Y[lutPos];
					float vecZ = zi - z + CELL_3D_Z[lutPos];

					float newDistance = (FastAbs(vecX) + FastAbs(vecY) + FastAbs(vecZ)) + (vecX * vecX + vecY * vecY + vecZ * vecZ);

					distance2 = fmax(fmin(distance2, newDistance), distance);
					distance = fmin(distance, newDistance);
				}
			}
		}
		break;

	default:
		break;
	}

	switch (mCellularReturnType)
	{
	case Distance2:
		return distance2 - 1;
	case Distance2Add:
		return distance2 + distance - 1;
	case Distance2Sub:
		return distance2 - distance - 1;
	case Distance2Mul:
		return distance2 * distance - 1;
	case Distance2Div:
		return distance / distance2 - 1;
	default:
		return 0;
	}
}

float UFastNoiseLibrary::GetCellular2D(float x, float y)
{
	x *= mFrequency;
	y *= mFrequency;

	switch (mCellularReturnType)
	{
	case CellValue:
	case NoiseLookup:
	case Distance:
		return SingleCellular2D(x, y);

	default:
		return SingleCellular2Edge2D(x, y);
	}
}

float UFastNoiseLibrary::SingleCellular2D(float x, float y)
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);

	float distance = 999999;
	int32 xc, yc;

	xc = yc = 0; // Silence compiler

	switch (mCellularDistanceFunction)
	{
	default:
	case Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = vecX * vecX + vecY * vecY;

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;

	case Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = (FastAbs(vecX) + FastAbs(vecY));

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;

	case Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = (FastAbs(vecX) + FastAbs(vecY)) + (vecX * vecX + vecY * vecY);

				if (newDistance < distance)
				{
					distance = newDistance;
					xc = xi;
					yc = yi;
				}
			}
		}
		break;
	}

	uint8 lutPos;
	switch (mCellularReturnType)
	{
	case CellValue:
		return ValCoord2D(mSeed, xc, yc);

	case NoiseLookup:
		//assert(mCellularNoiseLookup);

		lutPos = Index2D_256(0, xc, yc);
		return mCellularNoiseLookup->GetNoise2D(xc + CELL_2D_X[lutPos], yc + CELL_2D_Y[lutPos]);

	case Distance:
		return distance - 1;
	default:
		return 0;
	}
}

float UFastNoiseLibrary::SingleCellular2Edge2D(float x, float y)
{
	int32 xr = FastRound(x);
	int32 yr = FastRound(y);

	float distance = 999999;
	float distance2 = 999999;

	switch (mCellularDistanceFunction)
	{
	default:
	case Euclidean:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = vecX * vecX + vecY * vecY;

				distance2 = fmax(fmin(distance2, newDistance), distance);
				distance = fmin(distance, newDistance);
			}
		}
		break;

	case Manhattan:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = FastAbs(vecX) + FastAbs(vecY);

				distance2 = fmax(fmin(distance2, newDistance), distance);
				distance = fmin(distance, newDistance);
			}
		}
		break;

	case Natural:
		for (int32 xi = xr - 1; xi <= xr + 1; xi++)
		{
			for (int32 yi = yr - 1; yi <= yr + 1; yi++)
			{
				uint8 lutPos = Index2D_256(0, xi, yi);

				float vecX = xi - x + CELL_2D_X[lutPos];
				float vecY = yi - y + CELL_2D_Y[lutPos];

				float newDistance = (FastAbs(vecX) + FastAbs(vecY)) + (vecX * vecX + vecY * vecY);

				distance2 = fmax(fmin(distance2, newDistance), distance);
				distance = fmin(distance, newDistance);
			}
		}
		break;
	}

	switch (mCellularReturnType)
	{
	case Distance2:
		return distance2 - 1;
	case Distance2Add:
		return distance2 + distance - 1;
	case Distance2Sub:
		return distance2 - distance - 1;
	case Distance2Mul:
		return distance2 * distance - 1;
	case Distance2Div:
		return distance / distance2 - 1;
	default:
		return 0;
	}
}

void UFastNoiseLibrary::GradientPerturb3D(float& x, float& y, float& z)
{
	SingleGradientPerturb3D(0, mGradientPerturbAmp, mFrequency, x, y, z);
}

void UFastNoiseLibrary::GradientPerturbFractal3D(float& x, float& y, float& z)
{
	float Amp = mGradientPerturbAmp * mFractalBounding;
	float freq = mFrequency;
	int32 i = 0;

	SingleGradientPerturb3D(mPerm[0], Amp, mFrequency, x, y, z);

	while (++i < mOctaves)
	{
		freq *= mLacunarity;
		Amp *= mGain;
		SingleGradientPerturb3D(mPerm[i], Amp, freq, x, y, z);
	}
}

void UFastNoiseLibrary::SingleGradientPerturb3D(uint8 Offset, float warpAmp, float frequency, float& x, float& y, float& z)
{
	float xf = x * frequency;
	float yf = y * frequency;
	float zf = z * frequency;

	int32 x0 = FastFloor(xf);
	int32 y0 = FastFloor(yf);
	int32 z0 = FastFloor(zf);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;
	int32 z1 = z0 + 1;

	float xs, ys, zs;
	switch (mInterpolation)
	{
	default:
	case Linear:
		xs = xf - (float)x0;
		ys = yf - (float)y0;
		zs = zf - (float)z0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(xf - (float)x0);
		ys = InterpHermiteFunc(yf - (float)y0);
		zs = InterpHermiteFunc(zf - (float)z0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(xf - (float)x0);
		ys = InterpQuinticFunc(yf - (float)y0);
		zs = InterpQuinticFunc(zf - (float)z0);
		break;
	}

	int32 lutPos0 = Index3D_256(Offset, x0, y0, z0);
	int32 lutPos1 = Index3D_256(Offset, x1, y0, z0);

	float lx0x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	float ly0x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	float lz0x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	lutPos0 = Index3D_256(Offset, x0, y1, z0);
	lutPos1 = Index3D_256(Offset, x1, y1, z0);

	float lx1x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	float ly1x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	float lz1x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	float lx0y = Lerp(lx0x, lx1x, ys);
	float ly0y = Lerp(ly0x, ly1x, ys);
	float lz0y = Lerp(lz0x, lz1x, ys);

	lutPos0 = Index3D_256(Offset, x0, y0, z1);
	lutPos1 = Index3D_256(Offset, x1, y0, z1);

	lx0x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	ly0x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	lz0x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	lutPos0 = Index3D_256(Offset, x0, y1, z1);
	lutPos1 = Index3D_256(Offset, x1, y1, z1);

	lx1x = Lerp(CELL_3D_X[lutPos0], CELL_3D_X[lutPos1], xs);
	ly1x = Lerp(CELL_3D_Y[lutPos0], CELL_3D_Y[lutPos1], xs);
	lz1x = Lerp(CELL_3D_Z[lutPos0], CELL_3D_Z[lutPos1], xs);

	x += Lerp(lx0y, Lerp(lx0x, lx1x, ys), zs) * warpAmp;
	y += Lerp(ly0y, Lerp(ly0x, ly1x, ys), zs) * warpAmp;
	z += Lerp(lz0y, Lerp(lz0x, lz1x, ys), zs) * warpAmp;
}

void UFastNoiseLibrary::GradientPerturb2D(float& x, float& y)
{
	SingleGradientPerturb2D(0, mGradientPerturbAmp, mFrequency, x, y);
}

void UFastNoiseLibrary::GradientPerturbFractal2D(float& x, float& y)
{
	float Amp = mGradientPerturbAmp * mFractalBounding;
	float freq = mFrequency;
	int32 i = 0;

	SingleGradientPerturb2D(mPerm[0], Amp, mFrequency, x, y);

	while (++i < mOctaves)
	{
		freq *= mLacunarity;
		Amp *= mGain;
		SingleGradientPerturb2D(mPerm[i], Amp, freq, x, y);
	}
}

void UFastNoiseLibrary::SingleGradientPerturb2D(uint8 Offset, float warpAmp, float frequency, float& x, float& y)
{
	float xf = x * frequency;
	float yf = y * frequency;

	int32 x0 = FastFloor(xf);
	int32 y0 = FastFloor(yf);
	int32 x1 = x0 + 1;
	int32 y1 = y0 + 1;

	float xs, ys;
	switch (mInterpolation)
	{
	default:
	case Linear:
		xs = xf - (float)x0;
		ys = yf - (float)y0;
		break;

	case Hermite:
		xs = InterpHermiteFunc(xf - (float)x0);
		ys = InterpHermiteFunc(yf - (float)y0);
		break;

	case Quintic:
		xs = InterpQuinticFunc(xf - (float)x0);
		ys = InterpQuinticFunc(yf - (float)y0);
		break;
	}

	int32 lutPos0 = Index2D_256(Offset, x0, y0);
	int32 lutPos1 = Index2D_256(Offset, x1, y0);

	float lx0x = Lerp(CELL_2D_X[lutPos0], CELL_2D_X[lutPos1], xs);
	float ly0x = Lerp(CELL_2D_Y[lutPos0], CELL_2D_Y[lutPos1], xs);

	lutPos0 = Index2D_256(Offset, x0, y1);
	lutPos1 = Index2D_256(Offset, x1, y1);

	float lx1x = Lerp(CELL_2D_X[lutPos0], CELL_2D_X[lutPos1], xs);
	float ly1x = Lerp(CELL_2D_Y[lutPos0], CELL_2D_Y[lutPos1], xs);

	x += Lerp(lx0x, lx1x, ys) * warpAmp;
	y += Lerp(ly0x, ly1x, ys) * warpAmp;
}