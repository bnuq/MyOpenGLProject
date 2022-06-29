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

( 이미 XZ 평면에서는 이동했다고 생각, Y 축 방향으로도 일단 이동하는 게 맞지 )
- 메인 캐릭터가 중력에 의해 이동한다
- 이동하기 전 위치를 저장해둔다 => TempPos
  - Fall()
  - 이전 위치와, 갱신된 위치가 모두 존재
- 이후 Floor 중 하나와 충돌이 있는 지 확인을 한다
  - Collide()
    - 충돌을 하나라도 찾으면 검색 종료
    - 충돌 Floor 에 대해서 상태 변화
      - ChangeColor() 메서드 호출
      - Floor 객체 내부 멤버로 시간을 재기 시작해서, 셰이더에 정보를 넘긴다
        - 이후 일정 시간이 지나면 DeleteFloor() 호출
    - AnyCollision = true
- 충돌이 있다, AnyCollision == true
    - 캐릭터의 속도, 점프 횟수를 초기화 한다
      - Stay()
    - 그리고 위치를 충돌하기 전으로 돌린다
      - TempPos 를 위치로 다시 가진다
    

- 카메라가 캐릭터를 따라가고, 방향 설정
- Draw


Floor 입장에서 생각
처음에는 그냥 Draw 가 된다
그러다가 메인 캐릭터와 충돌 => Collision = true; 가 되면
    색깔을 변화시키는 메서드를 호출
        ChangeColor()
            시간이 지나면서, 변하는 값을 셰이더에 넘긴다
                일단은 Unirform 변수라 생각
                program 이 필요하겠고
            일정 시간이 지나면 GameMap 에서 해당 Floor 를 빼내야 한다
                그래야 더이상 Draw 도 안되고
                물리 충돌에 포함되지도 않는다
                    Hash Function 에 Floor 들을 저장
                    메인 캐릭터의 위치에서 일정 범위의 값으로 Floor 를 찾고
                    찾아내서 빼낼 수도 있다
            빼는 건 어디서 호출하지?
                메서드? 함수?
            지난 시간은 Floor 객체 멤버 값으로 저장?
        DeleteFloor(Hash Function)
            해시 함수에서 해당 Floor 를 찾고 없애는 작업
            소멸자까지 아예 호출해버려? 자동으로 호출되나?




카메라 멤버
    카메라 좌표계 x, y, z
        y = 항상 y 축과 동일
        x, z 는 마우스 입력에 따라 회전한다
    바라보는 벡터
        frontVec 기준으로 해서, pitch 만큼 회전

메인 캐릭터 멤버
    캐릭터 좌표계 x, y, z
        역시 y 는 y 축과 동일하다고 생각
            어차피 평면만 움직이니까

마우스가 회전 => 카메라 좌표계가 돈다
    도는 데, 카메라가 어떻게 도느냐?
    메인 캐릭터는 회전하지 않고 가만히 있는데
    
    1. 카메라 위치가 메인 캐릭터를 중심으로 돈다
       1. Position 을 yaw 각도만큼 y 축 방향 기준 회전
    2. frontVec 는 항상 메인 캐릭터를 향한다
       1. frontVec = normalize(mainChar->Position - Position)
    3. LeftVec 는 외적으로 구할 수 있지 않나?
       1. LeftVec = cross(Up, Front)
    4. Direcion 은 frontVec 에서 pitch 만 수정하면 된다

카메라가 돌고 난 이후
메인 캐릭터가 키입력을 받는데
    w
        카메라의 frontVec 방향과 메인 캐릭터의 frontVec 가 일치해야 한다
    s
    d
    a

    키를 동시에 누르는 경우에는 대각선으로 이동하잖아
    방향 키를 누른다 = 그쪽 방향으로 각을 더한다?

방향 키를 누르면 => 일단 카메라 좌표계 벡터 중 하나를 선택하고
캐릭터의 frontVec 가 그 벡터 방향으로 이동해야 하는 데
오른쪽 회전인지 왼쪽 회전인지 어떻게 구별하냐
    캐릭터의 frontVec 와 따라가야 하는 카메라 벡터를 외적해
    결과 벡터의 방향이 +y 면
        왼쪽 회전
            일치할 때까지 왼쪽방향으로 회전 시행

    결과 벡터의 방향이 -y 면
        오른쪽 회전
            일치할 때까지 오른쪽 방향으로 회전

            만약 키를 2개를 동시에 누르면 서로 일치할 때까지 회전을 시키니까
            자연스럽게 그 중간이 대각선으로 이동하게 된다