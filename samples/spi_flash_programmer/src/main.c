#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/sfp.h>

int main()
{
	return sfp_identify(DEVICE_DT_GET(DT_NODELABEL(sfp0)));
}
