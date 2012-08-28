// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ASH_SYSTEM_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_
#define ASH_SYSTEM_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_

#include "ash/ash_export.h"
#include "ash/system/tray/tray_background_view.h"
#include "ash/system/user/login_status.h"
#include "base/gtest_prod_util.h"
#include "ui/aura/event_filter.h"

namespace aura {
class LocatedEvent;
}

namespace gfx {
class ImageSkia;
}

namespace views {
class Label;
}

namespace ash {

namespace internal {
class StatusAreaWidget;
class WebNotificationButtonView;
class WebNotificationList;
class WebNotificationMenuModel;
class WebNotificationView;
}

// Status area tray for showing browser and app notifications. The client
// (e.g. Chrome) calls [Add|Remove|Update]Notification to create and update
// notifications in the tray. It can also implement Delegate to receive
// callbacks when a notification is removed (closed), clicked on, or a menu
// item is triggered.
//
// Note: These are not related to system notifications (i.e NotificationView
// generated by SystemTrayItem). Visibility of one notification type or other
// is controlled by StatusAreaWidget.

class ASH_EXPORT WebNotificationTray : public internal::TrayBackgroundView {
 public:
  class Delegate {
   public:
    virtual ~Delegate() {}

    // Called when the notification associated with |notification_id| is
    // removed (i.e. closed by the user).
    virtual void NotificationRemoved(const std::string& notifcation_id) = 0;

    // Request to disable the extension associated with |notification_id|.
    virtual void DisableExtension(const std::string& notifcation_id) = 0;

    // Request to disable notifications from the source of |notification_id|.
    virtual void DisableNotificationsFromSource(
        const std::string& notifcation_id) = 0;

    // Request to show the notification settings (|notification_id| is used
    // to identify the requesting browser context).
    virtual void ShowSettings(const std::string& notifcation_id) = 0;

    // Called when the notification body is clicked on.
    virtual void OnClicked(const std::string& notifcation_id) = 0;
  };

  explicit WebNotificationTray(internal::StatusAreaWidget* status_area_widget);
  virtual ~WebNotificationTray();

  // Called once to set the delegate.
  void SetDelegate(Delegate* delegate);

  // Add a new notification. |id| is a unique identifier, used to update or
  // remove notifications. |title| and |meesage| describe the notification text.
  // Use SetNotificationImage to set the icon image. If |extension_id| is
  // provided then 'Disable extension' will appear in a dropdown menu and the
  // id will be used to disable notifications from the extension. Otherwise if
  // |display_source| is provided, a menu item showing the source and allowing
  // notifications from that source to be disabled will be shown. All actual
  // disabling is handled by the Delegate.
  void AddNotification(const std::string& id,
                       const string16& title,
                       const string16& message,
                       const string16& display_source,
                       const std::string& extension_id);

  // Update an existing notification with id = old_id and set its id to new_id.
  void UpdateNotification(const std::string& old_id,
                          const std::string& new_id,
                          const string16& title,
                          const string16& message);

  // Remove an existing notification.
  void RemoveNotification(const std::string& id);

  // Set the notification image.
  void SetNotificationImage(const std::string& id,
                            const gfx::ImageSkia& image);

  // Set whether or not the popup notifications should be hidden.
  void SetHidePopupBubble(bool hide);

  // Updates tray visibility login status of the system changes.
  void UpdateAfterLoginStatusChange(user::LoginStatus login_status);

  // Returns true if the message center bubble is visible.
  bool IsMessageCenterBubbleVisible() const;

  // Returns true if the mouse is inside the notification bubble.
  bool IsMouseInNotificationBubble() const;

  // Overridden from TrayBackgroundView.
  virtual void SetShelfAlignment(ShelfAlignment alignment) OVERRIDE;
  virtual void AnchorUpdated() OVERRIDE;
  virtual string16 GetAccessibleName() OVERRIDE;

  // Overridden from internal::ActionableView.
  virtual bool PerformAction(const ui::Event& event) OVERRIDE;

  // Constants exposed for unit tests:
  static const size_t kMaxVisibleTrayNotifications;
  static const size_t kMaxVisiblePopupNotifications;

 private:
  class Bubble;
  class MessageCenterBubble;
  class PopupBubble;
  friend class internal::WebNotificationButtonView;
  friend class internal::WebNotificationMenuModel;
  friend class internal::WebNotificationList;
  friend class internal::WebNotificationView;
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, WebNotifications);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, WebNotificationPopupBubble);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest,
                           ManyMessageCenterNotifications);
  FRIEND_TEST_ALL_PREFIXES(WebNotificationTrayTest, ManyPopupNotifications);

  // Sends a remove request to the delegate.
  void SendRemoveNotification(const std::string& id);

  // Sends a remove request for all notifications to the delegate.
  void SendRemoveAllNotifications();

  // Disables all notifications matching notification |id|.
  void DisableByExtension(const std::string& id);
  void DisableByUrl(const std::string& id);

  // Requests the Delegate to the settings dialog.
  void ShowSettings(const std::string& id);

  // Called when a notification is clicked on. Event is passed to the Delegate.
  void OnClicked(const std::string& id);

  // Shows or updates the message center bubble and hides the popup bubble.
  void ShowMessageCenterBubble();

  // Hides the message center bubble if visible.
  void HideMessageCenterBubble();

  // Shows or updates the popup notification bubble if appropriate.
  void ShowPopupBubble();

  // Hides the notification bubble if visible.
  void HidePopupBubble();

  // Updates the tray icon and visibility.
  void UpdateTray();

  // As above but also updates any visible bubble.
  void UpdateTrayAndBubble();

  // Hides the specified bubble (called when |bubble| is closed from Views).
  void HideBubble(Bubble* bubble);

  // Testing accessors.
  size_t GetNotificationCountForTest() const;
  bool HasNotificationForTest(const std::string& id) const;
  void RemoveAllNotificationsForTest();
  size_t GetMessageCenterNotificationCountForTest() const;
  size_t GetPopupNotificationCountForTest() const;

  internal::WebNotificationList* notification_list() {
    return notification_list_.get();
  }
  MessageCenterBubble* message_center_bubble() const {
    return message_center_bubble_.get();
  }
  PopupBubble* popup_bubble() const { return popup_bubble_.get(); }

  scoped_ptr<internal::WebNotificationList> notification_list_;
  scoped_ptr<MessageCenterBubble> message_center_bubble_;
  scoped_ptr<PopupBubble> popup_bubble_;
  views::Label* count_label_;
  Delegate* delegate_;
  bool show_message_center_on_unlock_;

  DISALLOW_COPY_AND_ASSIGN(WebNotificationTray);
};

}  // namespace ash

#endif  // ASH_SYSTEM_NOTIFICATION_WEB_NOTIFICATION_TRAY_H_
