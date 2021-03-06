# Copyright 2014 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
import unittest

from telemetry.core import browser_options
from telemetry.core.backends.chrome import ios_browser_finder
from telemetry.unittest import test


class IosBrowserFinderUnitTest(unittest.TestCase):
  # TODO(baxley): Currently the tests require a device with Chrome running.
  # This should be stubbed out so it runs on any system, with no device
  # dependencies.
  @test.Enabled('ios')
  def testFindIosChrome(self):
    finder_options = browser_options.BrowserFinderOptions()
    browsers = ios_browser_finder.FindAllAvailableBrowsers(finder_options)
    self.assertTrue(browsers)
    for browser in browsers:
      self.assertEqual('ios-chrome', browser.browser_type)

if __name__ == '__main__':
  unittest.main()
