#include <array>
#include <iostream>

#include "stream_motion/stream.hpp"

void printIOs(const stream_motion::RobotStatusPacket& status)
{
  std::cout << "io_status: (";
  for (size_t i = 0; i < std::size(status.io_status); ++i)
  {
    std::cout << static_cast<int>(status.io_status[i]);
    if (i != std::size(status.io_status) - 1)
      std::cout << ", ";
  }
  std::cout << ")" << std::endl;
}

int main()
{
  // Create fresh connection
  stream_motion::StreamMotionConnection connection("10.10.10.2");

  // Configure GPIO
  std::array<stream_motion::GPIOControlConfig, 32> gpio_config{};

  const uint32_t start = 1;
  const uint32_t length = 1;

  // Write
  gpio_config[0].command_type = stream_motion::GPIOCommandType::IOCmd;
  gpio_config[0].gpio_type = static_cast<uint32_t>(stream_motion::IOType::F);
  gpio_config[0].start = start;
  gpio_config[0].length = length;
  // Read
  gpio_config[1].command_type = stream_motion::GPIOCommandType::IOState;
  gpio_config[1].gpio_type = static_cast<uint32_t>(stream_motion::IOType::F);
  gpio_config[1].start = start;
  gpio_config[1].length = length;

  if (!connection.configureGPIO(gpio_config))
  {
    std::cerr << "Failed to configure GPIO." << std::endl;
    return 1;
  }

  stream_motion::RobotStatusPacket status;

  // Start streaming
  connection.sendStartPacket();
  connection.getStatusPacket(status);

  // Send command with IOs on
  std::array<double, 9> empty_pos = { 0.0 };
  std::array<uint8_t, 256> io_command{};
  io_command[0] = 0xFF;  // Set config IOs to ON
  connection.sendCommand(empty_pos, false, io_command);

  // Get result and print
  connection.getStatusPacket(status);
  printIOs(status);

  // Stream the same command for a second (125 Hz)
  for (int i = 0; i < 125; ++i)
  {
    connection.sendCommand(empty_pos, false, io_command);
    connection.getStatusPacket(status);
  }

  // Send command with IOs off
  io_command[0] = 0x00;  // Set config IOs to OFF
  connection.sendCommand(empty_pos, false, io_command);

  // Get result and print
  connection.getStatusPacket(status);
  printIOs(status);

  // Stop streaming
  connection.sendStopPacket();
  return 0;
}
