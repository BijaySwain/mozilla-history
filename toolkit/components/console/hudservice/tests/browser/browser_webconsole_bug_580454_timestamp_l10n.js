/* vim:set ts=2 sw=2 sts=2 et: */
/* ***** BEGIN LICENSE BLOCK *****
 * Any copyright is dedicated to the Public Domain.
 * http://creativecommons.org/publicdomain/zero/1.0/
 *
 * Contributor(s):
 *  Patrick Walton <pcwalton@mozilla.com>
 *
 * ***** END LICENSE BLOCK ***** */

// Tests that appropriately-localized timestamps are printed.

Cu.import("resource://gre/modules/XPCOMUtils.jsm");
Cu.import("resource:///modules/HUDService.jsm");

function test() {
  const TEST_TIMESTAMP = 12345678;
  let date = new Date(TEST_TIMESTAMP);
  let localizedString = ConsoleUtils.timestampString(TEST_TIMESTAMP);
  isnot(localizedString.indexOf(date.getHours()), -1, "the localized " +
    "timestamp contains the hours");
  isnot(localizedString.indexOf(date.getMinutes()), -1, "the localized " +
    "timestamp contains the minutes");
  isnot(localizedString.indexOf(date.getSeconds()), -1, "the localized " +
    "timestamp contains the seconds");
  isnot(localizedString.indexOf(date.getMilliseconds()), -1, "the localized " +
    "timestamp contains the milliseconds");
}

