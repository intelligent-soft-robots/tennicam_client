#include <filesystem>
#include "gtest/gtest.h"
#include "tennicam_client/ball.hpp"
#include "tennicam_client/driver.hpp"
#include "tennicam_client/transform.hpp"

using namespace tennicam_client;

class TennicamClientTests : public ::testing::Test
{
};

TEST_F(TennicamClientTests, ball_serialization)
{
    const std::string segment_id = "tennicam_client_tests";

    shared_memory::clear_shared_memory(segment_id);

    long int ball_id = 5;
    std::array<double, 3> position;
    position.fill(3);
    std::array<double, 3> velocity;
    velocity.fill(4);
    long int time_stamp = 100;

    Ball in{ball_id, position, velocity, time_stamp};
    shared_memory::serialize(segment_id, segment_id, in);

    Ball out;
    shared_memory::deserialize(segment_id, segment_id, out);

    ASSERT_EQ(ball_id, out.get_ball_id());
    ASSERT_EQ(time_stamp, out.get_time_stamp());

    for (std::size_t index = 0; index < 3; index++)
    {
        ASSERT_EQ(position[index], out.get_position()[index]);
        ASSERT_EQ(velocity[index], out.get_velocity()[index]);
    }

    shared_memory::clear_shared_memory(segment_id);
}

TEST_F(TennicamClientTests, identity_transform)
{
    std::array<double, 3> translation;
    translation.fill(0);
    std::array<double, 3> rotation;
    rotation.fill(0);
    Transform t{translation, rotation};

    std::array<double, 3> in{1, 2, 3};

    std::array<double, 3> out = t.apply(in);

    for (std::size_t index = 0; index < 3; index++)
    {
        ASSERT_EQ(in[index], out[index]);
    }
}

TEST_F(TennicamClientTests, translation)
{
    std::array<double, 3> translation;
    translation.fill(1);
    std::array<double, 3> rotation;
    rotation.fill(0);
    Transform t{translation, rotation};

    std::array<double, 3> in{1, 2, 3};

    std::array<double, 3> out = t.apply(in);

    for (std::size_t index = 0; index < 3; index++)
    {
        ASSERT_EQ(in[index] + 1, out[index]);
    }
}

TEST_F(TennicamClientTests, axis_rotation)
{
    std::array<double, 3> translation;
    translation.fill(0);

    std::array<double, 3> rotation_x;
    rotation_x.fill(0);
    rotation_x[0] = M_PI / 2;
    Transform t_x{translation, rotation_x};
    std::array<double, 3> in_x{0, 1, 1};
    std::array<double, 3> out_x = t_x.apply(in_x);
    ASSERT_DOUBLE_EQ(out_x[0], 0.);
    ASSERT_DOUBLE_EQ(out_x[1], -1.);
    ASSERT_DOUBLE_EQ(out_x[2], 1.);

    std::array<double, 3> rotation_y;
    rotation_y.fill(0);
    rotation_y[1] = M_PI / 2;
    Transform t_y{translation, rotation_y};
    std::array<double, 3> in_y{1, 0, 1};
    std::array<double, 3> out_y = t_y.apply(in_y);
    ASSERT_DOUBLE_EQ(out_y[0], 1);
    ASSERT_DOUBLE_EQ(out_y[1], 0);
    ASSERT_DOUBLE_EQ(out_y[2], -1);

    std::array<double, 3> rotation_z;
    rotation_z.fill(0);
    rotation_z[2] = M_PI / 2;
    Transform t_z{translation, rotation_z};
    std::array<double, 3> in_z{1, 1, 0};
    std::array<double, 3> out_z = t_z.apply(in_z);
    ASSERT_DOUBLE_EQ(out_z[0], -1);
    ASSERT_DOUBLE_EQ(out_z[1], 1);
    ASSERT_DOUBLE_EQ(out_z[2], 0);
}

TEST_F(TennicamClientTests, parse_toml)
{
    // writting tmp config file
    std::filesystem::path tmp_file = std::filesystem::temp_directory_path();
    tmp_file /= "tennicam_client_tests_tmp";
    std::ofstream os;
    os.open(tmp_file.c_str());
    os << "[transform]" << std::endl
       << "translation = [0,1,2]" << std::endl
       << "rotation = [0.0,0.1,0.2]" << std::endl
       << "[server]" << std::endl
       << "hostname = \"127.0.0.1\"" << std::endl
       << "port = 7660" << std::endl;
    os.close();

    // parsing the file
    DriverConfig config = parse_toml(tmp_file.string());

    // checking config properly read
    ASSERT_EQ(config.server_port, 7660);
    ASSERT_TRUE(config.server_hostname == std::string("127.0.0.1"));
    for (std::size_t i = 0; i < 3; i++)
    {
        ASSERT_DOUBLE_EQ(config.translation[i], static_cast<double>(i));
        ASSERT_DOUBLE_EQ(config.rotation[i], static_cast<double>(i) * 0.1);
    }
}

TEST_F(TennicamClientTests, read_write_transform)
{
    const std::string segment_id = "tennicam_client_tests";
    shared_memory::clear_shared_memory(segment_id);

    // writting tmp config file
    std::filesystem::path tmp_file = std::filesystem::temp_directory_path();
    tmp_file /= "tennicam_client_tests_tmp";
    std::ofstream os;
    os.open(tmp_file.c_str());
    os << "[transform]" << std::endl
       << "translation = [0,1,2]" << std::endl
       << "rotation = [0.0,0.1,0.2]" << std::endl
       << "[server]" << std::endl
       << "hostname = \"127.0.0.1\"" << std::endl
       << "port = 7660" << std::endl;
    os.close();

    std::array<double, 3> translation;
    translation[0] = 0, translation[1] = 1, translation[2] = 2;
    std::array<double, 3> rotation;
    rotation[0] = 3, rotation[1] = 4, rotation[2] = 5;

    tennicam_client::write_transform_to_memory(
        segment_id, translation, rotation);

    std::tuple<std::array<double, 3>, std::array<double, 3>> t =
        tennicam_client::read_transform_from_memory(segment_id);

    std::array<double, 3> new_trans = std::get<0>(t);
    std::array<double, 3> new_rot = std::get<1>(t);

    for (std::size_t index = 0; index < 3; index++)
    {
        ASSERT_DOUBLE_EQ(translation[index], new_trans[index]);
        ASSERT_DOUBLE_EQ(rotation[index], new_rot[index]);
    }

    tennicam_client::update_transform_config_file(
        tmp_file.c_str(), new_trans, new_rot);

    tennicam_client::DriverConfig config =
        tennicam_client::parse_toml(tmp_file.c_str());

    for (std::size_t index = 0; index < 3; index++)
    {
        ASSERT_DOUBLE_EQ(translation[index], config.translation[index]);
        ASSERT_DOUBLE_EQ(rotation[index], config.rotation[index]);
    }

    ASSERT_EQ(config.server_port, 7660);
    ASSERT_STREQ(std::string("127.0.0.1").c_str(),
                 config.server_hostname.c_str());
}
