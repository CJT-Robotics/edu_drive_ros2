#include "PowerManagementBoard.h"
#include <iostream>
#include "can/canprotocol.h"
#include <unistd.h>
#include <cstring>

namespace edu
{

PowerManagementBoard::PowerManagementBoard(SocketCAN* can, bool verbosity)
{
  _init = false;
  _verbosity = verbosity;
  
  _voltage = 0.f;
  _current = 0.f;

  makeCanStdID(SYSID_PWRMGMT, NODEID_PWRMGMT, &_inputAddress, &_outputAddress, &_broadcastAddress);
  _cf.can_id = _inputAddress;
  if(verbosity)
    std::cout << "#PowerManagementBoard CAN Input ID: " << _inputAddress << " CAN Output ID: " << _outputAddress << std::endl;

  canid_t canidOutput = _outputAddress;

  setCANId(canidOutput);
  can->registerObserver(this);
  
  for(unsigned int i=0; i<500; i++)
  {
    if(_init) break;
    usleep(10000);
  }
}

PowerManagementBoard::~PowerManagementBoard()
{

}

float PowerManagementBoard::getVoltage()
{
  return _voltage;
}

float PowerManagementBoard::getCurrent()
{
  return _current;
}

void PowerManagementBoard::notify(struct can_frame* frame)
{
  if(frame->can_dlc==6)
  {
    //@ToDo: deserialize properly
    char reversedBytes[4] = {frame->data[4], frame->data[3], frame->data[2], frame->data[1]};
    if(frame->data[0] == 1)
    {
        // deserialize 4 bytes to a float
        
        std::memcpy(&_current, &reversedBytes, sizeof(_current));
    }
    else if(frame->data[0] == 2)
    {
        // deserialize 4 bytes to a float
        std::memcpy(&_voltage, &reversedBytes, sizeof(_voltage));
    }
    
    if(_verbosity)
      std::cout << "PowerManagement CANID " << _cf.can_id << " recerived data: V_pwr_mgmt=" << _voltage << "V I_pwr_mgmt=" << _current << "A" << std::endl;
      
    _init = true;
  }
}

} // namespace
