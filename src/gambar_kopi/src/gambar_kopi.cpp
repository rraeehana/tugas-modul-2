#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <turtlesim/srv/set_pen.hpp>

#include <algorithm>
#include <chrono>
#include <cmath>
#include <memory>
#include <thread>
#include <vector>

using namespace std::chrono_literals;

using Twist = geometry_msgs::msg::Twist;
using Pose = turtlesim::msg::Pose;
using SetPen = turtlesim::srv::SetPen;

struct Titik {
    double x;
    double y;
};

class GambarKopi : public rclcpp::Node {
public:
    GambarKopi() : Node("gambar_kopi") {
        publisher_ = create_publisher<Twist>(
            "/turtle1/cmd_vel", 10);

        subscriber_ = create_subscription<Pose>(
            "/turtle1/pose", 10,
            [this](const Pose::SharedPtr msg) {
                pose_ = *msg;
                pose_tersedia_ = true;
            });

        pen_client_ = create_client<SetPen>(
            "/turtle1/set_pen");
    }

    void tunggu_pose() {
        while (rclcpp::ok() && !pose_tersedia_) {
            rclcpp::spin_some(
                get_node_base_interface());
            std::this_thread::sleep_for(10ms);
        }
    }

    // Fungsi untuk mengatur warna, ketebalan, dan status pena (off=0 gambar, off=1 angkat)
    void atur_pen_warna(uint8_t r, uint8_t g, uint8_t b, uint8_t width, uint8_t off) {
        while (rclcpp::ok() && !pen_client_->wait_for_service(1s)) {
            RCLCPP_INFO(
                get_logger(),
                "Menunggu layanan set_pen...");
        }

        auto request = std::make_shared<SetPen::Request>();
        request->r = r;
        request->g = g;
        request->b = b;
        request->width = width;
        request->off = off;

        auto future = pen_client_->async_send_request(request);

        while (rclcpp::ok() && future.wait_for(100ms) != std::future_status::ready) {
            rclcpp::spin_some(get_node_base_interface());
        }
    }

    void ke_titik(const Titik& tujuan) {
        rclcpp::WallRate rate(30.0);

        while (rclcpp::ok()) {
            rclcpp::spin_some(
                get_node_base_interface());

            if (!pose_tersedia_) {
                rate.sleep();
                continue;
            }

            double dx = tujuan.x - pose_.x;
            double dy = tujuan.y - pose_.y;
            double jarak = std::hypot(dx, dy);

            Twist cmd;

            if (jarak < 0.035) {
                publisher_->publish(Twist{});
                break;
            }

            double sudut_tujuan = std::atan2(dy, dx);
            double error = sudut_tujuan - pose_.theta;

            while (error > M_PI) error -= 2.0 * M_PI;
            while (error < -M_PI) error += 2.0 * M_PI;

            if (std::abs(error) > 0.06) {
                cmd.angular.z =
                    std::clamp(3.0 * error, -2.0, 2.0);
            } else {
                cmd.linear.x =
                    std::min(1.2, 1.5 * jarak);
                cmd.angular.z =
                    std::clamp(2.0 * error, -1.0, 1.0);
            }

            publisher_->publish(cmd);
            rate.sleep();
        }

        publisher_->publish(Twist{});
        std::this_thread::sleep_for(100ms);
    }  

    void gambar() {
        tunggu_pose();
        if (!rclcpp::ok()) return;
        
        //---BAGIAN 0: Angkat pena dulu dari titik spawn awal ---
        RCLCPP_INFO(get_logger(), "Mengangkat pena.dari titik spawn..");
        atur_pen_warna(255, 255, 255, 3, 1); // Pena OFF

        //pindahin kura" ke titik awal tutup gelas
        Titik titik_awal_tutup = {4.20, 6.70};
        ke_titik(titik_awal_tutup);
        std::this_thread::sleep_for(200ms);


        // --- BAGIAN 1: Gambar Penutup Gelas (Outline Atas) ---
        RCLCPP_INFO(get_logger(), "Mengangkat pena...");
        atur_pen_warna(255, 255, 255, 3, 0); // Pena ON

        // Pena ON, warna putih

        const std::vector<Titik> lintasan_penutup = {
            {4.20, 6.70},  // ujung kiri bawah tutup
            {4.20, 7.10},
            {4.50, 7.10},
            {4.50, 7.60},
            {6.70, 7.60},  // bagian atas tutup rata
            {6.70, 7.10},
            {7.00, 7.10},
            {7.00, 6.70},
            {4.20, 6.70}   // tutup kembali ke awal penutup
        };

        for (const auto& titik : lintasan_penutup) {
            if (!rclcpp::ok()) break;
            ke_titik(titik);
        }

        // --- BAGIAN 2: Angkat Pena untuk Pindah Posisi (Tanpa Garis Bocor) ---
        RCLCPP_INFO(get_logger(), "Mengangkat pena...");
        atur_pen_warna(255, 255, 255, 3, 1); // Pena OFF

        Titik awal_badan = {4.40, 6.45};
        ke_titik(awal_badan);
        std::this_thread::sleep_for(200ms);

        // --- BAGIAN 3: Gambar Badan Gelas ---
        RCLCPP_INFO(get_logger(), "Menurunkan pena dan menggambar badan gelas...");
        atur_pen_warna(255, 255, 255, 3, 0); // Pena ON lagi

        const std::vector<Titik> lintasan_badan = {
            {6.80, 6.45},  // sisi kanan atas badan
            {6.50, 3.40},  // sisi kanan turun menyempit
            {4.70, 3.40},  // dasar bawah gelas
            {4.40, 6.45}   // sisi kiri naik kembali ke atas
        };

        for (const auto& titik : lintasan_badan) {
            if (!rclcpp::ok()) break;
            ke_titik(titik);
        }

        publisher_->publish(Twist{});

        RCLCPP_INFO(
            get_logger(),
            "Selesai menggambar outline gelas kopi!");
    }

private:
    rclcpp::Publisher<Twist>::SharedPtr publisher_;
    rclcpp::Subscription<Pose>::SharedPtr subscriber_;
    rclcpp::Client<SetPen>::SharedPtr pen_client_;

    Pose pose_;
    bool pose_tersedia_ = false;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);

    auto node = std::make_shared<GambarKopi>();

    node->gambar();

    rclcpp::shutdown();
    return 0;
}