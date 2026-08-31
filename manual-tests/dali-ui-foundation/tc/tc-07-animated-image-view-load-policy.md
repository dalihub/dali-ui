# 07. AnimatedImageView: LoadPolicy / ReleasePolicy / SyncLoading

SetLoadPolicy / GetLoadPolicy, SetReleasePolicy / GetReleasePolicy, SetSynchronousLoading / IsSynchronousLoading 동작을 확인한다.

## 화면 구성

- 중앙: 애니메이션 프리뷰 (200x200)
- 상태 라벨: LoadPolicy / ReleasePolicy / Sync / View 상태
- Loads 라벨: `Loads: N` — ResourceReadySignal이 발생한 횟수
- 버튼 행 1: Load IMMEDIATE / Load ATTACHED
- 버튼 행 2: Release DETACHED / Release DESTROYED / Release NEVER
- 버튼 행 3: Sync ON / Sync OFF
- 버튼 행 4: Remove View / Re-Add View / Play

> **Loads가 말해주는 것과 말해주지 못하는 것** (실측 2026-08-26, aiv-260825):
> ResourceReadySignal은 뷰의 리소스가 "준비됨"이 될 때마다 발생한다. Remove → Re-Add는
> **NEVER(캐시 재사용)든 DETACHED(리로드)든 똑같이 +1**을 만든다 — 준비의 출처(캐시/새
> 디코드)는 신호 층위에서 구분되지 않는다. 값 setter도 비주얼을 다시 만들므로 탭마다 +1이다.
> 그래서 이 카운터로 판정할 수 있는 것은 "로드/준비가 **언제** 일어났는가"(LoadPolicy·Sync·
> Reload)이고, NEVER vs DETACHED의 캐시 재사용 여부는 여전히 화면 밖 관측(프로파일러)의
> 영역이다.

## 테스트 1: ReleasePolicy NEVER

1. [Release: NEVER] 버튼을 탭한다 (setter 리빌드로 Loads +1)
2. [Remove View] 버튼을 탭한다
3. **기대 결과**: 애니메이션이 화면에서 사라짐. Loads는 그대로
4. [Re-Add View] 버튼을 탭한다
5. **기대 결과**: 즉시 복원됨. Loads +1 (준비 신호 — 캐시 재사용 여부는 이 신호로 판정 불가)

## 테스트 2: ReleasePolicy DETACHED

1. [Release: DETACHED] 버튼을 탭한다
2. [Remove View] -> [Re-Add View] 버튼을 탭한다
3. **기대 결과**: 다시 표시되고 Loads +1 (Re-Add에서만 오르고 Remove에서는 오르지 않음)

## 테스트 3: LoadPolicy IMMEDIATE vs ATTACHED

1. [Remove View]로 뷰를 씬에서 뺀 상태에서 [Load: IMMEDIATE]를 탭하고 Loads 값을 본다
2. **기대 결과**: IMMEDIATE는 씬에 붙기 전에 로드한다 — **Re-Add 전에 Loads가 증가**한다
3. [Re-Add View]를 탭한다
4. **기대 결과**: 표시가 복원된다
5. 반대로, 씬 밖에서 [Load: ATTACHED]를 탭하면 **Loads가 오르지 않고**, [Re-Add View]에서
   비로소 오른다

## 테스트 4: SynchronousLoading

1. [Sync ON] 버튼을 탭한다
2. **기대 결과**: Sync 라벨이 ON. 이후의 로드가 동기로 수행된다 — 액션 직후 한 번만
   읽어도 Loads가 이미 올라 있어야 한다

> Release: DESTROYED 버튼은 화면에 있으나 절차는 아직 정의하지 않았다 — DESTROYED의
> detach 의미(사양 결정 필요)가 ImageView 쪽과 함께 정해지면 절차를 추가한다.

## 통과 기준

- Remove 시 사라지고 Re-Add 시 재등장해야 한다 (픽셀), Re-Add에서 준비 신호가 와야 한다
  (Loads 증가 — **횟수는 계약이 아니다**: 같은 Re-Add가 엔진 캐시가 데워져 있으면 +1,
  콜드면 +2를 보고한다. 실측 2026-08-27 aiv-260825: tc-04를 먼저 방문해 같은 소재를
  데우면 5, 프로세스 첫 방문이면 6) — 캐시 재사용 vs 리로드 자체는 화면 관측 밖
  (신호가 두 경우를 같게 보고함)
- IMMEDIATE 정책: 씬 밖에서도 로드가 시작되어야 한다 (Re-Add 전 Loads 증가로 판정)
- ATTACHED 정책: 씬 밖에서는 로드하지 않아야 한다 (Re-Add까지 Loads 불변으로 판정)
- Sync ON 시 로드가 동기로 완료되어야 한다 (액션 직후 단발 읽기로 판정)
