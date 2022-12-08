// -*-c++-*--------------------------------------------------------------------
// Copyright 2022 Bernd Pfrommer <bernd.pfrommer@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <limits>

void usage()
{
  std::cout << "usage:" << std::endl;
  std::cout << "fast_rosbag_slice -i input_bag -o output_bag -s start_time -e stop_time "
            << std::endl;
}

static size_t process_bag(
  const std::string & inBagName, const std::string & outBagName, const double startTime,
  const double endTime)
{
  std::cout << "reading from bag: " << inBagName << std::endl;
  std::cout << "writing to bag: " << outBagName << std::endl;
  rosbag::Bag inBag;
  inBag.open(inBagName, rosbag::bagmode::Read);
  rosbag::Bag outBag;
  outBag.open(outBagName, rosbag::bagmode::Write);

  rosbag::View view(inBag);
  size_t numMessages(0);
  for (const rosbag::MessageInstance & m : view) {
    if (m.getTime().toSec() > endTime) {
      break;
    }
    if (m.getTime().toSec() >= startTime) {
      outBag.write(m.getTopic(), m.getTime(), m);
      numMessages++;
    }
  }
  inBag.close();
  outBag.close();
  return (numMessages);
}

int main(int argc, char ** argv)
{
  int opt;
  ros::Time::init();
  std::string inBag;
  std::string outBag;
  double startTime(0);
  double endTime(std::numeric_limits<double>::max());
  while ((opt = getopt(argc, argv, "i:o:s:e:h")) != -1) {
    switch (opt) {
      case 'i':
        inBag = optarg;
        break;
      case 'o':
        outBag = optarg;
        break;
      case 's':
        startTime = atof(optarg);
        break;
      case 'e':
        endTime = atof(optarg);
        break;
      case 'h':
        usage();
        return (-1);
      default:
        std::cout << "unknown option: " << opt << std::endl;
        usage();
        return (-1);
        break;
    }
  }
  if (inBag.empty() || outBag.empty()) {
    std::cout << "missing input or output bag name!" << std::endl;
    usage();
    return (-1);
  }
  const auto start = std::chrono::high_resolution_clock::now();

  size_t numMsg = process_bag(inBag, outBag, startTime, endTime);

  const auto end = std::chrono::high_resolution_clock::now();
  auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  std::cout << "total time: " << total_duration * 1e-6 << " s" << std::endl;
  std::cout << "message processing rate: " << numMsg * 1e6 / total_duration << " hz" << std::endl;

  return (0);
}
