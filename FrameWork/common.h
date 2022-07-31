/*
    ���������� ����ϴ� �������
    ���������� ����ϴ� ��ɵ���
    ��Ƽ� ������ ����
 */

/*  
    ��� ���� ������ �ѹ��� include ��Ű�� ���
    #pragma once �� windows ������ ����
 */
#ifndef __COMMON_H__    // if not define
#define __COMMON_H__    // define

#include <memory>
#include <string>
/* 
    std::optional<>
    � ���� �ְų� ���� ��츦 ������ ���� ǥ��
    ��� ���� ���� ���� �ְ�, ���� ���� �ִ� ��츦 �ٷ��
 */
#include <optional>

// �ܺ� ���̺귯�� �߰�
// �ٸ� ��� Ŭ���������� ����� �� �ְ� �ϱ� ���ؼ�
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <spdlog/spdlog.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/* 
    Ŭ���� �� ����Ʈ ������ ���� ��ũ��

    � Ŭ������ �����ϰ�
    � Ŭ������ ����Ű�� ����Ʈ ������ alias �� ����� ��ũ��
        
    ## = �� ���ڸ� ���δٴ� ��
*/
#define CLASS_PTR(klassName) \
class klassName; \
using klassName ## UPtr = std::unique_ptr<klassName>; \
using klassName ## Ptr = std::shared_ptr<klassName>; \
using klassName ## WPtr = std::weak_ptr<klassName>;

/* 
    �ؽ�Ʈ ������ �д� �Լ�

    ������ �д� ���� ������ ���� ������, ������ ���� �ִ�
        ���� nullptr �� ���������μ� �˸�
        ������ ������ ��� => Memory Leak ���輺

    std::optional<>
        ������ �д� �� �����ߴ� ��, �����ߴ� ���� ���ϵ� ��ü�� ���ؼ� �� �� �ִ�
        ������ ��쿡�� => string ���� �̿��ϰ� �� �� �ִ�
*/
std::optional<std::string> LoadTextFile(const std::string& filename);
// �� ���� ��� ���
glm::vec3 GetAttenuationCoeff(float distance);

#endif // __COMMON_H__