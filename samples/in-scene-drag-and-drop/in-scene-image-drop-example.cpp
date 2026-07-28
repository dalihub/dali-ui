/*
 * Copyright (c) 2026 Samsung Electronics Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 */

#include <dali-ui-foundation/dali-ui-foundation.h>
#include <algorithm>
#include <fstream>
#include <utility>

using namespace Dali;
using namespace Dali::Ui;

#ifndef RESOURCES_DIR
#define RESOURCES_DIR "./res/"
#endif

namespace
{
constexpr uint32_t BACKGROUND_COLOR      = 0x101820u;
constexpr uint32_t PANEL_COLOR           = 0x263238u;
constexpr uint32_t TARGET_ACCEPTED_COLOR = 0x2E7D32u;
constexpr uint32_t TARGET_REJECTED_COLOR = 0xB71C1Cu;
constexpr uint32_t TARGET_DROPPED_COLOR  = 0x0D47A1u;
constexpr auto     IMAGE_RESOURCE_URL_TYPE = "application/x-dali-image-resource-url";
constexpr auto     IMAGE_METADATA_TYPE     = "application/x-dali-image-metadata";

Dali::String ResolveImageUrl()
{
  constexpr const char* IMAGE_PATHS[] = {
    RESOURCES_DIR "source-image.svg",
    "samples/in-scene-drag-and-drop/res/source-image.svg",
    "res/source-image.svg",
  };

  for(const char* path : IMAGE_PATHS)
  {
    std::ifstream image(path);
    if(image.good())
    {
      return Dali::String(path);
    }
  }
  return Dali::String(IMAGE_PATHS[0]);
}

Label CreateLabel(const char* text, float width, float height, float fontSize)
{
  Label label = Label::New(text);
  label.SetRequestedWidth(width);
  label.SetRequestedHeight(height);
  label.SetParentOrigin(ParentOrigin::TOP_LEFT);
  label.SetPivot(Pivot::TOP_LEFT);
  label.SetSensitive(false);
  label.SetFontSize(fontSize);
  label.SetTextColor(Color::WHITE);
  label.SetHorizontalTextAlignment(Text::Alignment::CENTER);
  label.SetVerticalTextAlignment(Text::Alignment::CENTER);
  return label;
}
} // unnamed namespace

class InSceneImageDropExample : public ConnectionTracker
{
public:
  explicit InSceneImageDropExample(Application& application)
  : mApplication(application)
  {
    application.InitSignal().Connect(this, &InSceneImageDropExample::Create);
  }

private:
  void Create(Application application)
  {
    Window      window       = application.GetWindow();
    const auto  size         = window.GetPositionSize();
    const float windowWidth  = static_cast<float>(size.width);
    const float windowHeight = static_cast<float>(size.height);
    const float panelWidth   = std::max(120.0f, std::min(240.0f, (windowWidth - 96.0f) * 0.5f));
    const float panelHeight  = std::max(180.0f, std::min(260.0f, windowHeight - 260.0f));
    const float imageSize    = std::max(88.0f, std::min(180.0f, std::min(panelWidth - 32.0f, panelHeight - 72.0f)));
    const float panelTop     = 150.0f;
    mImageUrl                = ResolveImageUrl();

    window.SetBackgroundColor(UiColor(BACKGROUND_COLOR));

    mRoot = View::New();
    mRoot.SetRequestedWidth(windowWidth);
    mRoot.SetRequestedHeight(windowHeight);
    window.Add(mRoot);

    Label title = CreateLabel("IMAGE SOURCE TO TARGET", windowWidth - 40.0f, 54.0f, 26.0f);
    title.SetRequestedX(20.0f);
    title.SetRequestedY(24.0f);
    mRoot.Add(title);

    Label instruction = CreateLabel(
      "Long press the source image, then drag and drop it on the target",
      windowWidth - 40.0f,
      52.0f,
      17.0f);
    instruction.SetRequestedX(20.0f);
    instruction.SetRequestedY(82.0f);
    mRoot.Add(instruction);

    mSourcePanel = CreatePanel(32.0f, panelTop, panelWidth, panelHeight);
    mRoot.Add(mSourcePanel);

    Label sourceTitle = CreateLabel("SOURCE", panelWidth, 44.0f, 20.0f);
    mSourcePanel.Add(sourceTitle);

    mSourceImage = ImageView::New(mImageUrl);
    ConfigureImage(mSourceImage, imageSize);
    mSourceImage.SetSensitive(true);
    mSourceImage.SetRequestedX((panelWidth - imageSize) * 0.5f);
    mSourceImage.SetRequestedY(52.0f);
    mSourcePanel.Add(mSourceImage);

    mTarget = CreatePanel(windowWidth - 32.0f - panelWidth,
                          panelTop,
                          panelWidth,
                          panelHeight);
    mRoot.Add(mTarget);

    Label targetTitle = CreateLabel("TARGET", panelWidth, 44.0f, 20.0f);
    mTarget.Add(targetTitle);

    mTargetImage = ImageView::New();
    ConfigureImage(mTargetImage, imageSize);
    mTargetImage.SetRequestedX((panelWidth - imageSize) * 0.5f);
    mTargetImage.SetRequestedY(52.0f);
    mTargetImage.SetOpacity(0.0f);
    mTarget.Add(mTargetImage);

    mTargetPlaceholder = CreateLabel("DROP\nIMAGE\nHERE", imageSize, imageSize, 19.0f);
    mTargetPlaceholder.SetRequestedX((panelWidth - imageSize) * 0.5f);
    mTargetPlaceholder.SetRequestedY(52.0f);
    mTarget.Add(mTargetPlaceholder);

    mStatus = CreateLabel("Ready", windowWidth - 64.0f, 60.0f, 18.0f);
    mStatus.SetRequestedX(32.0f);
    mStatus.SetRequestedY(panelTop + panelHeight + 24.0f);
    mRoot.Add(mStatus);

    mOverlay = View::New();
    mOverlay.SetRequestedWidth(windowWidth);
    mOverlay.SetRequestedHeight(windowHeight);
    mOverlay.SetLayoutMode(LayoutMode::STANDALONE);
    mOverlay.SetUiScalePolicy(UiScalePolicy::DISABLED);
    mOverlay.SetParentOrigin(ParentOrigin::TOP_LEFT);
    mOverlay.SetPivot(Pivot::TOP_LEFT);
    mOverlay.SetSensitive(false);
    mRoot.Add(mOverlay);

    mDetector = DragAndDropDetector::New();
    ConfigureLongPressActivation();
    mDetector.AttachSource(mSourceImage);
    DragPayload imagePayload{
      IMAGE_METADATA_TYPE,
      Property::Value("DALi sample SVG image"),
      DragAndDropOperation::LINK};
    imagePayload.AddRepresentation(IMAGE_RESOURCE_URL_TYPE, Property::Value(mImageUrl));
    imagePayload.AddAllowedOperation(DragAndDropOperation::COPY);
    mDetector.SetSourcePayload(mSourceImage, std::move(imagePayload));
    mDetector.AttachTarget(mTarget);
    mDetector.SetDropProposalCallback(
      mTarget,
      DragAndDropDetector::DropProposalCallback::New(
        this,
        &InSceneImageDropExample::AcceptImage));
    mDetector.SetDragPreviewContainer(mOverlay);
    mDetector.SetDragPreviewCallbacks(
      DragAndDropDetector::DragPreviewFactory::New(
        this,
        &InSceneImageDropExample::CreatePreview),
      DragAndDropDetector::DragPreviewUpdater::New(
        this,
        &InSceneImageDropExample::PositionPreview),
      DragAndDropDetector::DragPreviewFinalizer::New(
        this,
        &InSceneImageDropExample::FinalizePreview));
    mDetector.StartedSignal().Connect(this, &InSceneImageDropExample::OnStarted);
    mDetector.DroppedSignal().Connect(this, &InSceneImageDropExample::OnDropped);
    mDetector.CancelledSignal().Connect(this, &InSceneImageDropExample::OnCancelled);
    mDetector.TargetFeedbackChangedSignal().Connect(
      this,
      &InSceneImageDropExample::OnTargetFeedbackChanged);
    mDetector.EndedSignal().Connect(this, &InSceneImageDropExample::OnEnded);
    window.KeyEventSignal().Connect(this, &InSceneImageDropExample::OnKeyEvent);
  }

  View CreatePanel(float x, float y, float width, float height)
  {
    View panel = View::New();
    panel.SetRequestedWidth(width);
    panel.SetRequestedHeight(height);
    panel.SetRequestedX(x);
    panel.SetRequestedY(y);
    panel.SetParentOrigin(ParentOrigin::TOP_LEFT);
    panel.SetPivot(Pivot::TOP_LEFT);
    panel.SetCornerRadius(16.0f);
    panel.SetBackgroundColor(UiColor(PANEL_COLOR));
    return panel;
  }

  void ConfigureImage(ImageView image, float size)
  {
    image.SetRequestedWidth(size);
    image.SetRequestedHeight(size);
    image.SetParentOrigin(ParentOrigin::TOP_LEFT);
    image.SetPivot(Pivot::TOP_LEFT);
    image.SetSensitive(false);
    image.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
  }

  void ConfigureLongPressActivation()
  {
    mDetector.SetDragActivationMode(DragActivationMode::LONG_PRESS);
    mDetector.SetDragStartThreshold(0.0f);
    mDetector.SetDragActivationConfiguration(
      Device::Class::MOUSE,
      {DragActivationMode::LONG_PRESS, 0.0f});
    mDetector.SetDragActivationConfiguration(
      Device::Class::TOUCH,
      {DragActivationMode::LONG_PRESS, 0.0f});
  }

  DropProposal AcceptImage(const DragAndDropEvent& event)
  {
    Dali::String imageUrl;
    Property::Value imageData;
    const bool canCopyImage =
      event.GetPayload().GetRepresentationData(IMAGE_RESOURCE_URL_TYPE, imageData) &&
      imageData.Get(imageUrl) &&
      imageUrl.Size() > 0u &&
      event.GetPayload().IsOperationAllowed(DragAndDropOperation::COPY);
    return canCopyImage
             ? DropProposal::Accept(IMAGE_RESOURCE_URL_TYPE, DragAndDropOperation::COPY)
             : DropProposal::Reject();
  }

  View CreatePreview(const DragAndDropEvent& event)
  {
    Dali::String imageUrl;
    Property::Value imageData;
    if(!event.GetPayload().GetRepresentationData(IMAGE_RESOURCE_URL_TYPE, imageData) ||
       !imageData.Get(imageUrl))
    {
      return View();
    }

    const Vector3 sourceSize =
      event.GetSource().GetCurrentProperty<Vector3>(Actor::Property::SIZE);
    ImageView preview = ImageView::New(imageUrl);
    preview.SetRequestedWidth(sourceSize.x);
    preview.SetRequestedHeight(sourceSize.y);
    preview.SetLayoutMode(LayoutMode::STANDALONE);
    preview.SetUiScalePolicy(UiScalePolicy::DISABLED);
    preview.SetParentOrigin(ParentOrigin::TOP_LEFT);
    preview.SetPivot(Pivot::TOP_LEFT);
    preview.SetPositionUsesPivotEnabled(true);
    preview.SetSensitive(false);
    preview.SetOpacity(0.86f);
    preview.SetScale(Vector3(1.06f, 1.06f, 1.0f));
    preview.SetFittingMode(Image::FittingMode::FIT_KEEP_ASPECT_RATIO);
    return preview;
  }

  void PositionPreview(View preview, const DragAndDropEvent& event)
  {
    preview.SetPivot(
      Vector3(event.GetSourceAnchor().x, event.GetSourceAnchor().y, 0.5f));
    preview.SetRequestedX(event.GetPreviewLocalPosition().x);
    preview.SetRequestedY(event.GetPreviewLocalPosition().y);
  }

  void FinalizePreview(View preview, const DragAndDropEvent& event)
  {
    preview.SetOpacity(0.0f);
    if(event.GetResult() == DragAndDropResult::NO_TARGET)
    {
      mStatus.SetText("Released outside the target - no image copied");
    }
  }

  void OnStarted(const DragAndDropEvent& event, DragAndDropDetector)
  {
    View source = event.GetSource();
    source.SetOpacity(0.35f);
    mStatus.SetText("Dragging image - move over the target");
  }

  void OnDropped(const DragAndDropEvent& event, DragAndDropDetector)
  {
    if(event.GetTarget() != mTarget)
    {
      return;
    }

    Dali::String imageUrl;
    Property::Value imageData;
    const DropProposal& proposal = event.GetDropProposal();
    if(proposal.GetOperation() == DragAndDropOperation::COPY &&
       event.GetSelectedRepresentationData(imageData) &&
       imageData.Get(imageUrl))
    {
      mTargetImage.SetResourceUrl(imageUrl);
      mTargetImage.SetOpacity(1.0f);
      mTargetPlaceholder.SetOpacity(0.0f);
      mHasDroppedImage = true;
      mStatus.SetText("Dropped image resource URL with COPY proposal");
    }
  }

  void OnCancelled(const DragAndDropEvent&, DragAndDropDetector)
  {
    mStatus.SetText("Drag cancelled");
  }

  void OnTargetFeedbackChanged(const DragAndDropEvent& event, DragAndDropDetector)
  {
    if(event.GetTargetStatus() == DragAndDropTargetStatus::ACCEPTED)
    {
      mTarget.SetBackgroundColor(UiColor(TARGET_ACCEPTED_COLOR));
      mStatus.SetText("Target selected image resource URL + COPY");
    }
    else if(event.GetTargetStatus() == DragAndDropTargetStatus::REJECTED)
    {
      mTarget.SetBackgroundColor(UiColor(TARGET_REJECTED_COLOR));
    }
    else
    {
      RestoreTargetColor();
    }
  }

  void OnEnded(const DragAndDropEvent& event, DragAndDropDetector)
  {
    View source = event.GetSource();
    source.SetOpacity(1.0f);
    RestoreTargetColor();
  }

  void RestoreTargetColor()
  {
    mTarget.SetBackgroundColor(
      UiColor(mHasDroppedImage ? TARGET_DROPPED_COLOR : PANEL_COLOR));
  }

  void OnKeyEvent(Window, KeyEvent event)
  {
    if(event.GetState() != KeyEvent::DOWN)
    {
      return;
    }

    if(IsKey(event, Dali::DALI_KEY_ESCAPE) || IsKey(event, Dali::DALI_KEY_BACK))
    {
      if(mDetector.IsDragging() || mDetector.IsDragActivationPending())
      {
        mDetector.CancelDrag();
      }
      else
      {
        mApplication.Quit();
      }
    }
  }

  Application&        mApplication;
  DragAndDropDetector mDetector;
  View                mRoot;
  View                mSourcePanel;
  ImageView           mSourceImage;
  View                mTarget;
  ImageView           mTargetImage;
  Label               mTargetPlaceholder;
  Label               mStatus;
  View                mOverlay;
  Dali::String        mImageUrl;
  bool                mHasDroppedImage{false};
};

int DALI_EXPORT_API main(int argc, char** argv)
{
  Application             application = Application::New(&argc, &argv);
  InSceneImageDropExample example(application);
  application.MainLoop();
  return 0;
}
