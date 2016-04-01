#include "fsexp.hpp"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

using std::complex;

complex<double> fima_final(complex<double> z)
{
	complex<double> c, e;
	complex<double> Zo = complex<double>(.31813150520476413, 1.3372357014306895);
	complex<double> R = complex<double>(1.0779614375280, -.9465409639480);
	complex<double> a2 = 0.5 / (Zo - 1.0);
	complex<double> a3 = (a2 + 1.0 / 6.) / (Zo * Zo - 1.0);
	complex<double> a4 = (a2 / 2.0 + a2 * a2 / 2.0 + a3 + 1.0 / 24.0) / (Zo * Zo * Zo - 1.0);
	complex<double> a5 =
	    (0.5 * a2 * a2 + a2 / 6.0 + a2 * a3 + a3 / 2.0 + a4 + 1.0 / 120.0) / (Zo * Zo * Zo * Zo - 1.0);
	complex<double> Li = 2.0 * M_PI * complex<double>(0.0, 1.0);
	complex<double> b0 = complex<double>(0.1223, -0.02370);
	e = exp(Zo * z + R);
	c = Zo + e * (1.0 + e * (a2 + e * (a3 + e * (a4 + e * a5))) + b0 * exp(Li * z));
	return c;
}

complex<double> tai3_final(complex<double> z)
{
	int K = 50, k;
	complex<double> DER3[51] = {complex<double>(0.37090658903228507226, 1.33682167078891400713),
	                            complex<double>(0.03660096537598455518, 0.13922215389950498565),
	                            complex<double>(-0.16888431840641535131, 0.09718533619629270148),
	                            complex<double>(-0.12681315048680869007, -0.11831628767028627702),
	                            complex<double>(0.04235809310323926380, -0.10520930088320722129),
	                            complex<double>(0.05848306393563178218, -0.00810224524496080435),
	                            complex<double>(0.02340031665294847393, 0.01807777011820375229),
	                            complex<double>(0.00344260984701375092, 0.01815103755635914459),
	                            complex<double>(-0.00803695814441672193, 0.00917428467034995393),
	                            complex<double>(-0.00704695528168774229, -0.00093958506727472686),
	                            complex<double>(-0.00184617963095305509, -0.00322342583181676459),
	                            complex<double>(0.00054064885443097391, -0.00189672061015605498),
	                            complex<double>(0.00102243648088806748, -0.00055968657179243165),
	                            complex<double>(0.00064714396398048754, 0.00025980661935827123),
	                            complex<double>(0.00010444455593372213, 0.00037199472598828116),
	                            complex<double>(-0.00011178535404343476, 0.00016786687552190863),
	                            complex<double>(-0.00010630158710808594, 0.00002072200033125881),
	                            complex<double>(-0.00005078098819110608, -0.00003575913005741248),
	                            complex<double>(-0.00000314742998690270, -0.00003523185937587781),
	                            complex<double>(0.00001347661344130504, -0.00001333034137448205),
	                            complex<double>(0.00000980239082395275, 0.00000047607184151673),
	                            complex<double>(0.00000355493475454698, 0.00000389816212201278),
	                            complex<double>(-0.00000021552652645735, 0.00000296273413237997),
	                            complex<double>(-0.00000131673903627820, 0.00000097381354534333),
	                            complex<double>(-0.00000083401960806066, -0.00000018663858711081),
	                            complex<double>(-0.00000022869610981361, -0.00000037497716770031),
	                            complex<double>(0.00000005372584613379, -0.00000023060136585176),
	                            complex<double>(0.00000011406656653786, -0.00000006569510293486),
	                            complex<double>(0.00000006663595460757, 0.00000002326630571343),
	                            complex<double>(0.00000001396786846375, 0.00000003315118300198),
	                            complex<double>(-0.00000000684890556421, 0.00000001713041981611),
	                            complex<double>(-0.00000000916619598268, 0.00000000403886083652),
	                            complex<double>(-0.00000000502933384276, -0.00000000222121299478),
	                            complex<double>(-0.00000000084484352792, -0.00000000273668661113),
	                            complex<double>(0.00000000070086729861, -0.00000000124687683156),
	                            complex<double>(0.00000000070558101710, -0.00000000021962577544),
	                            complex<double>(0.00000000035900951951, 0.00000000018774741308),
	                            complex<double>(0.00000000005248658571, 0.00000000021201177126),
	                            complex<double>(-0.00000000006264758835, 0.00000000009059171879),
	                            complex<double>(-0.00000000005333473585, 0.00000000001006078866),
	                            complex<double>(-0.00000000002432138144, -0.00000000001506937008),
	                            complex<double>(-0.00000000000331880379, -0.00000000001544700067),
	                            complex<double>(0.00000000000501652570, -0.00000000000658967459),
	                            complex<double>(0.00000000000401214135, -0.00000000000036708383),
	                            complex<double>(0.00000000000158629111, 0.00000000000119885992),
	                            complex<double>(0.00000000000019668766, 0.00000000000106532662),
	                            complex<double>(-0.00000000000036355730, 0.00000000000047229527),
	                            complex<double>(-0.00000000000029920206, 0.00000000000001251827),
	                            complex<double>(-0.00000000000010305550, -0.00000000000009571381),
	                            complex<double>(-0.00000000000000910369, -0.00000000000007087680),
	                            complex<double>(0.00000000000002418310, -0.00000000000003240337)};
	complex<double> s = 0.0, t = 1.0;
	z -= complex<double>(0., 3.);
	z /= 2.0;
	for (k = 0; k < K; ++k)
	{
		s += DER3[k] * t;
		t *= z;
	}
	return s;
}
complex<double> maclo_final(complex<double> z)
{
	int K = 100, k;
	double d[110] = {0.30685281944005469058, 1.18353470251664338875, 1.58593285160678321155, 1.36629265207672068172,
	                 1.36264601823980036066, 1.21734246689515424045, 1.10981816083559525765, 0.96674692974769849130,
	                 0.84089872598668435888, 0.71353210966804747617, 0.60168548504001373445, 0.49928574281440518678,
	                 0.41140086629121763728, 0.33506195665178500898, 0.27104779243942234146, 0.21728554054610033086,
	                 0.17311050207880035456, 0.13690016038526570119, 0.10765949732729711286, 0.08413804539743192923,
	                 0.06542450487497340761, 0.05060001212013485322, 0.03895655493977817629, 0.02985084640296329153,
	                 0.02277908979501017117, 0.01730960309240666892, 0.01310389615589767874, 0.00988251130733762764,
	                 0.00742735935367278347, 0.00556296426263720549, 0.00415334478103463346, 0.00309116153137843543,
	                 0.00229387529664008653, 0.00169729976398295653, 0.00125245885041635465, 0.00092172809095368547,
	                 0.00067661152429638357, 0.00049544127485341987, 0.00036192128589181518, 0.00026376927786672476,
	                 0.00019180840045267570, 0.00013917553105723647, 0.00010077412023867018, 0.00007281884753121133,
	                 0.00005251474516228446, 0.00003779882770351268, 0.00002715594536867241, 0.00001947408515177282,
	                 0.00001394059355016322, 0.00000996213949015693, 0.00000710713872292710, 0.00000506199803708578,
	                 0.00000359960968975399, 0.00000255569149787694, 0.00000181175810338313, 0.00000128245831538430,
	                 0.00000090647322737496, 0.00000063980422418981, 0.00000045095738191441, 0.00000031741772125007,
	                 0.00000022312521183625, 0.00000015663840476155, 0.00000010982301013230, 0.00000007690305934973,
	                 0.00000005378502675604, 0.00000003757126131521, 0.00000002621429405247, 0.00000001826909956818,
	                 0.00000001271754463425, 0.00000000884310192977, 0.00000000614230041407, 0.00000000426177146865,
	                 0.00000000295386817285, 0.00000000204522503591, 0.00000000141464900426, 0.00000000097750884878,
	                 0.00000000067478454029, 0.00000000046535930671, 0.00000000032062550784, 0.00000000022069891976,
	                 0.00000000015177557961, 0.00000000010428189463, 0.00000000007158597119, 0.00000000004909806710,
	                 0.00000000003364531769, 0.00000000002303635851, 0.00000000001575933679, 0.00000000001077213757,
	                 0.00000000000735717912, 0.00000000000502077719, 0.00000000000342362421, 0.00000000000233271256,
	                 0.00000000000158818623, 0.00000000000108046566, 0.00000000000073450488, 0.00000000000049894945,
	                 0.00000000000033868911, 0.00000000000022973789, 0.00000000000015572383, 0.00000000000010548054,
	                 0.00000000000007139840, 0.00000000000004829557, 0.00000000000003264619, 0.00000000000002205299,
	                 0.00000000000001488731, 0.00000000000001004347, 0.00000000000000677124, 0.00000000000000456225,
	                 0.00000000000000307196, 0.00000000000000206720};
	complex<double> s = 0.0;
	complex<double> z2 = z / 2.0;
	complex<double> t = 1.0;
	for (k = 0; k <= K; ++k)
	{
		s += d[k] * t;
		t *= z2;
	}
	return s + log(z + 2.0);
}

complex<double> fima(complex<double> z)
{
	double x = z.real();
	double y = z.imag();
	if (y < 0.2379 * x)
		return exp(fima(z - 1.));
	return fima_final(z);
}
complex<double> tai3(complex<double> z)
{
	double x = z.real();
	if (x > 0.5)
		return exp(tai3(z - 1.0));
	if (x < -0.5)
		return log(tai3(z + 1.0));
	return tai3_final(z);
}
complex<double> maclo_abs(complex<double> z)
{
	double x = z.real();
	complex<double> c;
	if (x > 0.5)
	{
		c = z - 1.0;
		c = maclo_abs(c);
		c = exp(c);
		return c;
	}
	if (x < -0.5)
		return log(maclo_abs(z + 1.0));
	return maclo_final(z);
}
complex<double> fsexp(complex<double> z)
{
	double y = z.imag();
	if (y > 4.5)
		return fima(z);
	if (y > 1.5)
		return tai3(z);
	if (y > -1.5)
		return maclo_abs(z);
	if (y > -4.5)
		return conj(tai3(conj(z)));
	return conj(fima(conj(z)));
}