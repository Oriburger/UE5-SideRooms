<aside>
💡 24/02/23 오후 2:31 최초 작성
</aside>

- [자세한 소개 자료!!](https://www.miricanvas.com/ko/v/12wsqn1)

  

# 1. 실행 파일
---
<aside>
🚨 개발이 한창 진행중인 버전이기에 많이 불안정합니다. 양해 부탁드립니다.
</aside>

- [2024/02/23자 빌드](https://soongsilac-my.sharepoint.com/:u:/g/personal/uss425_soongsil_ac_kr/EZCCNjOzwKJJnx5o_9KzlyoByznvFTKsWIy59hRS2WGVjg?e=A8woZY)
- [2024/02/24자 빌드](https://soongsilac-my.sharepoint.com/:u:/g/personal/uss425_soongsil_ac_kr/EW3JwgqjMtlNvT52rzBfSAQBWhevHTgcI5MZpeKSJr9ngQ?e=Ifbtql)
    ```
    - 적의 속도 점점 빨라짐
    - 다리에 총을 쏘면 3초간 1/2속도
    ```

# 2. 게임 소개
---
- 백룸/바디캠st  +  멀티플레이 호러 FPS
- 규칙 
    ```
    1. 최대 4인의 멀티플레이  (LAN만 지원, 하마치 가능) 
    
    2. 로비에서 호스트가 엔터키로 게임을 시작
    
    3. 각 플레이어는 각자의 백룸에서 미션 액터를 탐색
    
    4. 모든 플레이어가 죽지 않고 액터를 모으면 승리
    ```
    

# 3. 그 외
---
### 3-1. 구현 사항
- 기본
    - [x]  캐릭터 기본
    - [x]  총기 기본
    - [ ]  데미지 이벤트 (~ing)
- 게임 플레이
    - [x]  미션 기본
    - [x]  상호작용
    - [ ]  난이도 점진적 증가
    - [ ]  맵 절차적 생성
- 멀티플레이
    - [x]  LAN 기반 접속 로직
    - [ ]  게임 진행 RPC (~ing)
    - [ ]  세션 기반 로직
- 시네마틱
    - [ ]  데미지 애니메이션 연출
    - [ ]  사망 연출
    - [ ]  클리어 연출

### 3-2. 오류 내역
- 간헐적 크래시
- 로비 메시지 UI가 사라지지 않음 
