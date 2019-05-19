//
//@**************************** class DHTLeaf ******************************
// 
// This class encapsulates a temp/humidity sensor that publishes measured
// environment values to MQTT
//

#include <DHT12.h>

class Dht12Leaf : public AbstractTempLeaf
{
public:
  DHT12 dht12;
 
  Dht12Leaf(String name, pinmask_t pins) : AbstractTempLeaf(name, pins) {
    ENTER(L_INFO);
    LEAVE;
  }

  void setup(void) {
    ENTER(L_NOTICE);
    Leaf::setup();
    dht12.begin();
    LEAVE;
  }

  bool poll(float *h, float *t, const char **status) 
  {
    *h = dht12.readHumidity();
    *t = dht12.readTemperature();
    if (isnan(*h) || isnan(*t)) {
      ALERT("Failed to read from DHT12 sensor");
      *status = "readfail";
      return false;
    }
    *status = "ok";
    return true;
  }
  
};

// local Variables:
// mode: C++
// c-basic-offset: 2
// End: