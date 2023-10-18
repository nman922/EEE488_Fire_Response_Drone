#ifndef IRSENSOR_HH_
#define IRSENSOR_HH_

#include <gz/sensors/Sensor.hh>
#include <gz/sensors/SensorTypes.hh>
#include <gz/transport/Node.hh>

namespace custom
{
    class IRSensor : public gz::sensors::Sensor
    {
        public: virtual bool Load(const sdf::Sensor &_sdf) override;

        public: virtual bool Update(const std::chrono::steady_clock::duration &_now) override;

        public: void NewPosition(const gz::math::Vector3d &_pos);

        public: const gz::math::Vector3d &Position() const;

        public: void FirePosition(const gz::math::Vector3d &_pos);

        private: gz::math::Vector3d prevPos{std::nan(""), std::nan(""), std::nan("")};

        private: gz::sensors::NoisePtr noise{nullptr};

        private: gz::transport::Node node;

        private: gz::transport::Node::Publisher pub;
    };
}

#endif