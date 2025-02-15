﻿#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
//#include <string> // Thêm thư viện string để làm việc với chuỗi

// Cấu trúc đạn
struct Bullet {
	float x, y;
	bool active;
};

// Cấu trúc đạn của tàu địch
struct EnemyBullet {
	float x, y;
	bool active;
};

// Cấu trúc tàu địch
struct Enemy {
	float x, y;
	float speedX;
	bool active;
	bool hasShot; // Cờ kiểm tra xem tàu địch đã bắn chưa
	std::vector<EnemyBullet> bullets; // Danh sách đạn của tàu địch
};

// Các thông số chung
const int screenWidth = 800;
const int screenHeight = 600;
const float planeWidth = 0.1f;
const float planeHeight = 0.1f;
const float playerSpeed = 0.0005f;
const float bulletSpeed = 0.001f; // Tốc độ đạn
//int enemyDestroyedCount = 0; // Biến đếm số tàu địch bị tiêu diệt

// Tọa độ máy bay người chơi
float planeX = 0.0f;
float planeY = -0.8f;

bool playerActive = true; // Cờ kiểm tra tàu người chơi có còn hoạt động không
bool hasPlayerBeenDestroyed = false;

// Danh sách đạn
std::vector<Bullet> bullets;
bool canShoot = true;

// Danh sách tàu địch
std::vector<Enemy> enemies;

// Hàm vẽ máy bay người chơi
void drawPlane() {
	// Vẽ đầu máy bay 
	glBegin(GL_TRIANGLES);
	glColor3f(0.0f, 1.0f, 0.0f); // Màu xanh lá cây
	glVertex2f(planeX, planeY + planeHeight / 2);  // Đỉnh của đầu máy bay
	glVertex2f(planeX - planeWidth / 4, planeY);  // Góc trái dưới
	glVertex2f(planeX + planeWidth / 4, planeY);  // Góc phải dưới
	glEnd();

	// Vẽ thân máy bay
	glBegin(GL_QUADS);
	glVertex2f(planeX - planeWidth / 4, planeY);   // Góc trái dưới của thân
	glVertex2f(planeX + planeWidth / 4, planeY);   // Góc phải dưới của thân
	glVertex2f(planeX + planeWidth / 4, planeY - planeHeight / 2); // Góc phải trên của thân
	glVertex2f(planeX - planeWidth / 4, planeY - planeHeight / 2); // Góc trái trên của thân
	glEnd();

	// Vẽ cánh máy bay
	glBegin(GL_TRIANGLES);
	glVertex2f(planeX - planeWidth, planeY - planeHeight / 4);  // Góc trái dưới của cánh
	glVertex2f(planeX + planeWidth, planeY - planeHeight / 4);  // Góc phải dưới của cánh
	glVertex2f(planeX, planeY + planeHeight / 4); // Đỉnh của cánh
	glEnd();

	// Vẽ phần lửa đuôi
	glBegin(GL_TRIANGLES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(planeX - planeWidth / 8, planeY - planeHeight / 2);  // Góc trái dưới của đuôi
	glVertex2f(planeX + planeWidth / 8, planeY - planeHeight / 2);  // Góc phải dưới của đuôi
	glVertex2f(planeX, planeY - planeHeight);  // Đỉnh của đuôi
	glEnd();
}

// Hàm vẽ đạn
void drawBullets() {
	glColor3f(1.0f, 0.0f, 0.0f); // Màu đỏ
	glPointSize(6.0f); // Kích thước viên đạn
	glBegin(GL_POINTS);
	for (auto& bullet : bullets) {
		if (bullet.active) {
			glVertex2f(bullet.x, bullet.y);
		}
	}
	glEnd();
}

// Hàm vẽ đạn của tàu địch
void drawEnemyBullets() {
	glColor3f(0.0f, 0.0f, 1.0f); // Màu xanh dương
	glPointSize(6.0f); // Kích thước viên đạn
	glBegin(GL_POINTS);
	for (auto& enemy : enemies) {
		for (auto& bullet : enemy.bullets) {
			if (bullet.active) {
				glVertex2f(bullet.x, bullet.y);
			}
		}
	}
	glEnd();
}

// Hàm cập nhật vị trí đạn của người chơi
void updateBullets() {
	for (auto& bullet : bullets) {
		if (bullet.active) {
			bullet.y += bulletSpeed;
			if (bullet.y > 1.0f) {
				bullet.active = false; // Vô hiệu hóa khi đạn ra khỏi màn hình
			}
		}
	}
}

// Hàm bắn đạn của người chơi
void shootBullet() {
	Bullet newBullet = { planeX, planeY + 0.1f, true };
	bullets.push_back(newBullet);
}

// Hàm tàu địch bắn đạn
void enemyShootBullet(Enemy& enemy) {
	if (playerActive) { // Only shoot if the player is still active
		if (abs(enemy.x - planeX) < 0.05f) {
			if (!enemy.hasShot) {
				EnemyBullet newBullet = { enemy.x, enemy.y - 0.05f, true };
				enemy.bullets.push_back(newBullet);
				enemy.hasShot = true;
			}
		}
		else {
			enemy.hasShot = false;
		}
	}
}

// Cập nhật đạn của tàu địch
void updateEnemyBullets() {
	for (auto& enemy : enemies) {
		if (enemy.active) {
			for (auto& bullet : enemy.bullets) {
				if (bullet.active) {
					bullet.y -= bulletSpeed; // Đạn di chuyển xuống dưới

					// Nếu đạn ra ngoài màn hình, vô hiệu hóa nó
					if (bullet.y < -1.0f) {
						bullet.active = false;
					}
				}
			}
		}
	}
}

// Hàm vẽ tàu địch
void drawEnemyPlane() {
	for (auto& enemy : enemies) {
		if (enemy.active) {
			// Vẽ đầu máy bay
			glBegin(GL_TRIANGLES);
			glColor3f(1.0f, 0.0f, 0.0f); // Màu đỏ cho tàu địch
			glVertex2f(enemy.x, enemy.y - planeHeight / 2);  // Đỉnh của đầu máy bay
			glVertex2f(enemy.x - planeWidth / 4, enemy.y);  // Góc trái dưới
			glVertex2f(enemy.x + planeWidth / 4, enemy.y);  // Góc phải dưới
			glEnd();

			// Vẽ thân máy bay
			glBegin(GL_QUADS);
			glVertex2f(enemy.x - planeWidth / 4, enemy.y);   // Góc trái dưới của thân
			glVertex2f(enemy.x + planeWidth / 4, enemy.y);   // Góc phải dưới của thân
			glVertex2f(enemy.x + planeWidth / 4, enemy.y + planeHeight / 2); // Góc phải trên của thân
			glVertex2f(enemy.x - planeWidth / 4, enemy.y + planeHeight / 2); // Góc trái trên của thân
			glEnd();

			// Vẽ cánh máy bay
			glBegin(GL_TRIANGLES);
			glVertex2f(enemy.x - planeWidth, enemy.y + planeHeight / 4);  // Góc trái dưới của cánh
			glVertex2f(enemy.x + planeWidth, enemy.y + planeHeight / 4);  // Góc phải dưới của cánh
			glVertex2f(enemy.x, enemy.y - planeHeight / 4); // Đỉnh của cánh
			glEnd();

			// Vẽ phần lửa của máy bay
			glBegin(GL_TRIANGLES);
			glVertex2f(enemy.x - planeWidth / 8, enemy.y + planeHeight / 2);  // Góc trái dưới của đuôi
			glVertex2f(enemy.x + planeWidth / 8, enemy.y + planeHeight / 2);  // Góc phải dưới của đuôi
			glVertex2f(enemy.x, enemy.y + planeHeight);  // Đỉnh của đuôi
			glEnd();
		}
	}
}


// Hàm cập nhật vị trí tàu địch và bắn đạn
void updateEnemyPlane() {
	for (auto& enemy : enemies) {
		if (enemy.active) {
			enemy.x += enemy.speedX;
			if (enemy.x < -1.0f || enemy.x > 1.0f) {
				enemy.speedX = -enemy.speedX; // Đổi hướng khi chạm biên
			}

			// Tàu địch bắn khi trùng trục X với máy bay người chơi
			enemyShootBullet(enemy);
		}
	}
}

// Khởi tạo danh sách tàu địch
void initializeEnemies(int count) {
	enemies.clear();
	srand(static_cast<unsigned int>(time(0))); // Seed ngẫu nhiên

	// Giới hạn số lượng tàu địch tối đa là 5
	count = std::min(count, 5); // Đảm bảo không tạo quá 5 tàu địch

	for (int i = 0; i < count; ++i) {
		// Tạo vị trí random cho tàu địch
		float x = static_cast<float>(rand()) / RAND_MAX * (2.0f - planeWidth) - (1.0f - planeWidth / 2);
		float y = static_cast<float>(rand()) / RAND_MAX * (0.5f - planeHeight) + (1.0f - 0.5f);

		// Random tốc độ ngang từ playerSpeed * 0.5f đến playerSpeed * 0.8f
		float speedX = (static_cast<float>(rand()) / RAND_MAX * (playerSpeed * 0.3f) + playerSpeed * 0.5f);
		if (rand() % 2 == 0) speedX = -speedX; // Random hướng

		// Thêm tàu địch vào danh sách
		enemies.push_back({ x, y, speedX, true, false });
	}
}

// Hàm kiểm tra va chạm giữa đạn và tàu địch
void checkCollision() {
	for (size_t i = 0; i < enemies.size(); ++i) {
		for (size_t j = 0; j < bullets.size(); ++j) {
			if (enemies[i].active && bullets[j].active) {
				if (bullets[j].x > enemies[i].x - planeWidth / 2 &&
					bullets[j].x < enemies[i].x + planeWidth / 2 &&
					bullets[j].y > enemies[i].y - planeHeight &&
					bullets[j].y < enemies[i].y) {

					enemies[i].active = false;
					bullets[j].active = false;
					//enemyDestroyedCount++;
				}
			}
		}
		// Kiểm tra va chạm giữa tàu địch và máy bay người chơi
		if (enemies[i].active && playerActive) {
			// Kiểm tra va chạm giữa tàu địch và máy bay người chơi (bounding box)
			if (enemies[i].x > planeX - planeWidth / 2 &&
				enemies[i].x < planeX + planeWidth / 2 &&
				enemies[i].y > planeY - planeHeight &&
				enemies[i].y < planeY) {

				// Máy bay người chơi và tàu địch va chạm
				if (!hasPlayerBeenDestroyed) {
					std::cout << "Player destroyed by enemy plane!" << std::endl;
					hasPlayerBeenDestroyed = true;  // Đặt cờ để ngừng in thông báo
				}
				playerActive = false;  // Ngừng vẽ và cập nhật máy bay người chơi
				enemies[i].active = false;  // Ngừng vẽ và cập nhật tàu địch
			}
		}
	}
	// Kiểm tra va chạm giữa đạn của tàu địch và máy bay người chơi
	for (auto& enemy : enemies) {
		for (size_t j = 0; j < enemy.bullets.size(); ++j) {
			if (enemy.bullets[j].active && playerActive) {
				if (enemy.bullets[j].x > planeX - planeWidth / 2 &&
					enemy.bullets[j].x < planeX + planeWidth / 2 &&
					enemy.bullets[j].y > planeY - planeHeight &&
					enemy.bullets[j].y < planeY) {

					if (!hasPlayerBeenDestroyed) {
						std::cout << "Player destroyed by enemy bullet!" << std::endl;
						hasPlayerBeenDestroyed = true;  // Đặt cờ để ngừng in thông báo
					}
					playerActive = false;  // Ngừng vẽ và cập nhật máy bay người chơi
					enemy.bullets[j].active = false;
				}
			}
		}
	}
}

// Hàm Main()
int main() {
	if (!glfwInit()) {
		std::cerr << "GLFW Initialization failed!" << std::endl;
		return -1;
	}

	// Tạo cửa sổ OpenGL
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Shooter Game", nullptr, nullptr);
	if (!window) {
		std::cerr << "GLFW Window creation failed!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Khởi tạo tàu địch
	initializeEnemies(5);

	while (!glfwWindowShouldClose(window)) {
		// Xóa màn hình
		glClear(GL_COLOR_BUFFER_BIT);

		// Nếu playerActive là false, ngừng vẽ và cập nhật
		if (playerActive) {
			// Cập nhật và vẽ máy bay người chơi
			drawPlane();

			// Cập nhật và vẽ viên đạn của người chơi
			updateBullets();
			drawBullets();
		}

		// Cập nhật và vẽ tàu địch
		updateEnemyPlane();
		drawEnemyPlane();

		// Cập nhật và vẽ viên đạn của tàu địch
		updateEnemyBullets();
		drawEnemyBullets();

		// Kiểm tra va chạm giữa đạn và tàu địch
		checkCollision();

		// Xử lý phím di chuyển
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS && playerActive) {
			planeX -= playerSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS && playerActive) {
			planeX += playerSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && playerActive) {
			planeY += playerSpeed;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && playerActive) {
			planeY -= playerSpeed;
		}

		// Giới hạn di chuyển của máy bay
		if (planeX - planeWidth / 2 < -1.0f) planeX = -1.0f + planeWidth / 2;
		if (planeX + planeWidth / 2 > 1.0f) planeX = 1.0f - planeWidth / 2;
		if (planeY - planeHeight < -1.0f) planeY = -1.0f + planeHeight;
		if (planeY > 1.0f) planeY = 1.0f;

		// Bắn đạn
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && playerActive) {
			if (canShoot) {
				shootBullet();
				canShoot = false;
			}
		}
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) {
			canShoot = true;
		}

		// Swap buffers và xử lý sự kiện
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}