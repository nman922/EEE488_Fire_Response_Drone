#include <math.h>

#include <gz/msgs/double.pb.h>

#include <gz/common/Console.hh>
#include <gz/msgs/Utility.hh>
#include <gz/sensors/Noise.hh>
#include <gz/sensors/Util.hh>

#include "IRSensor.hh"

using namespace custom;

bool IRSensor::Load(const sdf::Sensor &_sdf)
{
    auto type = gz::sensors::customType(_sdf);
    if("irsensor" != type)
    {
        gzerror << "Trying to load [ir] sensor, but got type [" << type << "] instead." << std::endl;
        return false;
    }

    gz::sensors::Sensor::Load(_sdf);

    this->pub = this->node.Advertise<gz::msgs::Double>(this->Topic());

    if(!_sdf.Element()->HasElement("gz:irsensor"))
    {
        gzdbg << "No custom configuration for [" << this->Topic() << "]" << std::endl;
        return true;
    }

    auto customElem = _sdf.Element()->GetElement("gz:irsensor");

    if(!customElem->HasElement("noise"))
    {
        gzdbg << "No noise for [" << this->Topic() << "]" << std::endl;
        return true;
    }

    sdf::Noise noiseSdf;
    noiseSdf.Load(customElem->GetElement("noise"));
    this->noise = gz::sensors::NoiseFactory::NewNoiseModel(noiseSdf);
    if(nullptr == this->noise)
    {
        gzerr << "Failed to load noise." << std::endl;
        return false;
    }

    return true;
}

bool IRSensor::Update(const std::chrono::steady_clock::duration &_now)
{
    gz::msgs::Double msg;
    *msg.mutable_header()->mutable_stamp() = gz::msgss::Convert(_now);
    auto fram = msg.mutable_header()->add_data();
    frame->set_key("frame_id")
    frame->add_value(this->Name());

    this->totalDistance = this->noise->Apply(this->totalDistance);

    msg.set_data(this->totalDistance);

    this->AddSequence(msg.mutable_header());
    this->pub.Publish(msg);

    return true;
}

void IRSensor::NewPosition(const gz::math::Vector3d &_pos)
{
    if(!isnan(this->prevPos.X()))
    {
        this->totalDistance += this->prevPos.Distance(_pos);
    }
    this->prevPos = _pos;
}

const gz::math::Vector3d &IRSensor::Position() const
{
    return this->prevPos;
}