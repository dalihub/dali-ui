# 25. ImageView: PlaceholderUrl

Placeholder는 본 이미지가 없거나 최초 이미지를 로딩 중일 때 표시한다.
ResourceURL을 빈 값으로 지우면 지정한 Placeholder로 돌아간다.
이미 준비된 이미지의 URL을 교체하면 새 이미지가 준비될 때까지 이전 이미지를 유지한다.
로드 실패 시에는 Placeholder 대신 broken image를 표시한다.

## 테스트 1: 빈 URL에서 Placeholder 표시

1. [Set Placeholder] → [Clear Image]를 탭한다.
2. Placeholder가 표시되고 유지되는지 확인한다.
3. [Clear Image]를 다시 탭해도 Placeholder가 유지되는지 확인한다.
4. [Clear Placeholder]를 탭하면 빈 이미지 상태가 되는지 확인한다.
5. [Set Placeholder]를 탭하면 다시 Placeholder가 표시되는지 확인한다.

## 테스트 2: 정상 이미지 로드와 초기화

1. Placeholder가 표시된 상태에서 [Switch to A]를 탭한다.
2. 정상 로드 완료 후 A 이미지가 표시되는지 확인한다.
3. [Clear Image]를 탭하면 Placeholder로 돌아가는지 확인한다.
4. [Switch to B] → [Clear Image]에서도 같은 동작을 확인한다.

## 테스트 3: 이미지 교체와 실패

1. A 이미지가 표시된 상태에서 [Set Placeholder]를 탭한다.
2. [Switch to B]를 탭한다. 로딩 중 이전 이미지를 유지하고 완료 후 B로 교체한다.
3. [Switch to Missing]을 탭한다. 실패 후 Placeholder가 아닌 broken image를 표시한다.
4. 실패 상태에서 [Set Placeholder]를 탭해도 broken image가 유지되는지 확인한다.
5. [Clear Image]를 탭하면 Placeholder가 표시되는지 확인한다.

## 통과 기준

- PlaceholderURL 설정과 해제 값이 getter에 반영된다.
- 본 이미지 URL이 비어 있으면 Placeholder를 표시한다.
- 정상 로드 완료 시 Placeholder를 제거한다.
- URL 교체 중에는 이전 이미지를 유지한다.
- 로드 실패 시에는 broken image를 표시한다.
- Placeholder URL을 해제하면 빈 본 이미지 URL에서는 아무 이미지도 표시하지 않는다.

캐시나 빠른 로딩으로 중간 상태가 화면에 나타나지 않을 수 있다.
