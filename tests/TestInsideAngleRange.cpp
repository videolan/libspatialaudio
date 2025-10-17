#undef NDEBUG
#include <cassert>

#include <Tools.h>

int main() {
	assert(spaudio::insideAngleRange(45., -180., 180., 1e-6));
	assert(spaudio::insideAngleRange(-30., -90., 90., 1e-6));
	assert(spaudio::insideAngleRange(-110., -120., -100.));
	assert(spaudio::insideAngleRange(0., -90., 90., 0.));
	assert(spaudio::insideAngleRange(91., -90., 90., 0.) == false);
	assert(spaudio::insideAngleRange(95., -90., 90., 5.));
	assert(spaudio::insideAngleRange(95., 90., -90., 0.));
	assert(spaudio::insideAngleRange(85., 90., -90., 0.) == false);
	assert(spaudio::insideAngleRange(85., 90., -90., 5.));
	assert(spaudio::insideAngleRange(0., 0., 0., 0.));
	assert(spaudio::insideAngleRange(1., 0., 0., 0.) == false);
	assert(spaudio::insideAngleRange(180., 180., 180., 0.));
	assert(spaudio::insideAngleRange(180., -180., -180., 0.));
	assert(spaudio::insideAngleRange(179., -180., -180., 0.) == false);
	assert(spaudio::insideAngleRange(175., 180., 180., 5.));
	assert(spaudio::insideAngleRange(170., 180., 180., 5.) == false);
	assert(spaudio::insideAngleRange(180., -180., 180., 0.));
	assert(spaudio::insideAngleRange(270., -180., 180., 0.));
	assert(spaudio::insideAngleRange(-90., -180., 180., 0.));
	assert(spaudio::insideAngleRange(0., -180., 180., 0.));
}
