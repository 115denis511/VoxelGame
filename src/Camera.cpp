#include "Camera.h"

engine::Camera::Camera() {
    setMode(CameraMode::FIRST_PERSON_VIEW);

    m_yaw = 0.0f;
	m_pitch = 0.0f;
	m_distance = 2.f;

	m_cameraPosition = glm::vec3(0.f);
	glm::vec3 front;
	front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	front.y = sin(glm::radians(m_pitch));
	front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_cameraTarget = glm::normalize(front);
	m_cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	m_cameraRight = glm::normalize(glm::cross(m_cameraTarget, m_cameraUp));
}

glm::mat4 engine::Camera::getLookAt() {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_lookAt;
}

void engine::Camera::updateLookAt() {
    std::lock_guard<std::mutex> lock(m_mutex);

    glm::vec3 front;
	front.x = cos(glm::radians(m_pitch)) * cos(glm::radians(m_yaw));
	front.y = sin(glm::radians(m_pitch));
	front.z = cos(glm::radians(m_pitch)) * sin(glm::radians(m_yaw));
	m_cameraTarget = glm::normalize(front);
	
	m_cameraRight = glm::normalize(glm::cross(m_cameraTarget, g_worldUp));
	m_cameraUp = glm::normalize(glm::cross(m_cameraRight, m_cameraTarget));

    switch (m_cameraMode) {
	case CameraMode::FIRST_PERSON_VIEW :
		m_lookAt = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraTarget, g_worldUp);
		break;

	case CameraMode::THIRD_PERSON_VIEW :
		m_lookAt = glm::lookAt(m_cameraPosition + m_cameraTarget * m_distance, m_cameraPosition, g_worldUp);
		break;
	
	default:
		m_lookAt = glm::lookAt(m_cameraPosition, m_cameraPosition + m_cameraTarget, g_worldUp);
		break;
	}
}

void engine::Camera::setMode(CameraMode mode) {
    m_cameraMode = mode;
	switch (mode) {
	case CameraMode::FIRST_PERSON_VIEW:
		m_modeMultiplier = 1.f;
		break;
	
	case CameraMode::THIRD_PERSON_VIEW:
		m_modeMultiplier = -1.f;
		break;
	
	default:
		m_modeMultiplier = 1.f;
		break;
	}
}

void engine::Camera::setYaw(GLfloat yaw) {
	m_yaw = yaw;
}

void engine::Camera::setPitch(GLfloat pitch) {
	m_pitch = pitch;

	if (m_pitch > 89.0f)
		m_pitch = 89.0f;
	if (m_pitch < -89.0f)
		m_pitch = -89.0f;
}

void engine::Camera::setDistance(GLfloat distance) {
	m_distance = distance;
}

const glm::vec3 engine::Camera::getPosition() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cameraPosition;
}

const glm::vec3 engine::Camera::getRight() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cameraRight * m_modeMultiplier;
}

const glm::vec3 engine::Camera::getFront() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cameraTarget * m_modeMultiplier;
}

const glm::vec3 engine::Camera::getUp() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cameraUp;
}

void engine::Camera::moveRotation(GLfloat yaw, GLfloat pitch) {
	setYaw(m_yaw + yaw * m_modeMultiplier);
	setPitch(m_pitch + pitch);
}

void engine::Camera::moveRotation(const glm::vec2 &offset) {
	setYaw(m_yaw + offset.x * m_modeMultiplier);
	setPitch(m_pitch + offset.y);
}

void engine::Camera::movePosition(const glm::vec3 &offset) {
    std::lock_guard<std::mutex> lock(m_mutex);
	m_cameraPosition += offset;
}
