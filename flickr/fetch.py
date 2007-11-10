#!/usr/bin/env python

import sys
from xml import xpath
import xml.dom.minidom as minidom
import urllib

api_url = "http://api.flickr.com/services/rest/"
api_key = "47d801c03cd569142aad928fac0d09a4"

class Photo(object):
  def __init__(self, dom_el):
    for i in ("id", "owner", "secret", "farm", "server", "title"):
      setattr(self, "_%s" % i, dom_el.getAttribute(i))

  def GetURL(self):
    return ("http://farm%s.static.flickr.com"
            "/%s/%s_%s.jpg"
            % (self._farm, self._server, self._id, self._secret))


def search(tags, sort):
  data = {
    "method": "flickr.photos.search",
    "api_key": "47d801c03cd569142aad928fac0d09a4",
    "tags": tags,
    "sort": sort,
    "group_id": "31917163@N00",
  }

  url = "%s?%s" % (api_url, urllib.urlencode(data))
  photoxml = urllib.urlopen(url).read()
  doc = minidom.parseString(photoxml)
  for photo_el in xpath.Evaluate("//photo", doc):
    photo = Photo(photo_el)
    print photo.GetURL()

search("blue", "interestingness-desc")
