Context
    Programs
        character 를 그리는 프로그램
        floor 를 그리는 프로그램
            floor 마다 가지는 색깔을 이용해서, 색을 결정한다
    
    Mesh
        character 를 그리는 메쉬
            character 를 위한 Material 을 가지고 있다
                character diffuse       texture
                character specular      texture

        floor 를 그리는 메쉬
            floor 를 위한 Material 을 가지고 있다
                floor diffuse
                    흰색 단색 이미지 -> 단색 텍스처
                floor specular
                    흰색 단색 이미지 -> 단색 텍스처

    Camera
        하나만 존재

    Light
        일단 하나만 존재

    Box
    => 충돌 감지 가능
        Mesh 는 박스 메쉬를 사용하는 데
        Box 마다 다른 텍스처를 가진다? => 그냥 그릴 때 마다 할당하면 되지 않나?
        Draw 할 때 프로그램에 할당하는 방식으로 진행하자

        Character
            하나만 존재
            키보드, 마우스 입력을 받았을 수 있다
                Move(int key)
                Jump()
                Rotate(float angle)
                    멤버 변수의 상태를 갱신
            
            Render() 과정에서
                임의의 바닥과 충돌을 했는 지 확인
                    Collide()
                    충돌하지 않았다면 떨어지는 것 계산
                        Fall()
                    충돌했다면, 상태 초기화가 필요하다
                        Stay()

            Draw()
                CharProgram 사용
                BoxMesh 사용
                CharTexture 사용
                메인 캐릭터의 위치와 회전 값을 이용 => ModelTransform
                카메라 세팅 사용 => view, projection

            
        Floor
            맵의 바닥을 이루는 박스들 => 일단 2개 존재
            여러 개가 존재 => GPU Instancing 으로 그린다
                일단 2개만 있다고 생각

            임의의 색깔을 가지며
            상태 변수가 존재
                닿았는 지 여부를 저장
                    true 가 되면, 시간을 재는 함수 발동
                    이후 사라져야 한다

            시간을 재는 함수가 있어야 하지 않나?
            메인 캐릭터와 충돌을 했을 때



Rendering 과정에서 하는 일을 천천히 생각

- 일단 충돌이 있는 지 확인을 한다
  - 캐릭터와 Floor 사이에서 충돌을 확인
  - 충돌이 있으면
    - 캐릭터 상태 초기화
      - Stay()
    - 충돌 Floor 에 대해서 상태 변화
      - Collision = true
      - 시간을 재기 시작해서, 셰이더에 정보를 넘긴다
  - 충돌이 없으면
    - 캐릭터 계속 낙하
      - Fall
- 카메라가 캐릭터를 따라가고, 방향 설정
- Draw