// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_RENDERER_HOST_CHROME_EXTENSION_MESSAGE_FILTER_H_
#define CHROME_BROWSER_RENDERER_HOST_CHROME_EXTENSION_MESSAGE_FILTER_H_

#include <string>

#include "base/sequenced_task_runner_helpers.h"
#include "content/public/browser/browser_message_filter.h"

class GURL;
class Profile;
struct ExtensionHostMsg_APIActionOrEvent_Params;
struct ExtensionHostMsg_DOMAction_Params;
struct ExtensionMsg_ExternalConnectionInfo;

namespace base {
class FilePath;
}

namespace extensions {
class InfoMap;
}

// This class filters out incoming Chrome-specific IPC messages from the
// extension process on the IPC thread.
class ChromeExtensionMessageFilter : public content::BrowserMessageFilter {
 public:
  ChromeExtensionMessageFilter(int render_process_id, Profile* profile);

  // content::BrowserMessageFilter methods:
  virtual bool OnMessageReceived(const IPC::Message& message) OVERRIDE;
  virtual void OverrideThreadForMessage(
      const IPC::Message& message,
      content::BrowserThread::ID* thread) OVERRIDE;

 private:
  friend class content::BrowserThread;
  friend class base::DeleteHelper<ChromeExtensionMessageFilter>;

  virtual ~ChromeExtensionMessageFilter();

  void OnCanTriggerClipboardRead(const GURL& origin, bool* allowed);
  void OnCanTriggerClipboardWrite(const GURL& origin, bool* allowed);

  // TODO(jamescook): Move these functions into the extensions module. Ideally
  // this would be in extensions::ExtensionMessageFilter but that will require
  // resolving the MessageService and ActivityLog dependencies on src/chrome.
  // http://crbug.com/339637
  void OnOpenChannelToExtension(int routing_id,
                                const ExtensionMsg_ExternalConnectionInfo& info,
                                const std::string& channel_name,
                                bool include_tls_channel_id,
                                int* port_id);
  void OpenChannelToExtensionOnUIThread(
      int source_process_id,
      int source_routing_id,
      int receiver_port_id,
      const ExtensionMsg_ExternalConnectionInfo& info,
      const std::string& channel_name,
      bool include_tls_channel_id);
  void OnOpenChannelToNativeApp(int routing_id,
                                const std::string& source_extension_id,
                                const std::string& native_app_name,
                                int* port_id);
  void OpenChannelToNativeAppOnUIThread(int source_routing_id,
                                        int receiver_port_id,
                                        const std::string& source_extension_id,
                                        const std::string& native_app_name);
  void OnOpenChannelToTab(int routing_id, int tab_id,
                          const std::string& extension_id,
                          const std::string& channel_name, int* port_id);
  void OpenChannelToTabOnUIThread(int source_process_id, int source_routing_id,
                                  int receiver_port_id,
                                  int tab_id, const std::string& extension_id,
                                  const std::string& channel_name);
  void OnGetExtMessageBundle(const std::string& extension_id,
                             IPC::Message* reply_msg);
  void OnGetExtMessageBundleOnBlockingPool(
      const base::FilePath& extension_path,
      const std::string& extension_id,
      const std::string& default_locale,
      IPC::Message* reply_msg);
  void OnExtensionCloseChannel(int port_id, const std::string& error_message);
  void OnAddAPIActionToExtensionActivityLog(
      const std::string& extension_id,
      const ExtensionHostMsg_APIActionOrEvent_Params& params);
  void OnAddBlockedCallToExtensionActivityLog(
      const std::string& extension_id,
      const std::string& function_name);
  void OnAddDOMActionToExtensionActivityLog(
      const std::string& extension_id,
      const ExtensionHostMsg_DOMAction_Params& params);
  void OnAddEventToExtensionActivityLog(
      const std::string& extension_id,
      const ExtensionHostMsg_APIActionOrEvent_Params& params);

  const int render_process_id_;

  // The Profile associated with our renderer process.  This should only be
  // accessed on the UI thread!
  Profile* profile_;

  scoped_refptr<extensions::InfoMap> extension_info_map_;

  DISALLOW_COPY_AND_ASSIGN(ChromeExtensionMessageFilter);
};

#endif  // CHROME_BROWSER_RENDERER_HOST_CHROME_EXTENSION_MESSAGE_FILTER_H_