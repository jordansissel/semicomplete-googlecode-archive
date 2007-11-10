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

def get_group_id(group):
  data = {
    "method": "flickr.groups.search",
    "api_key": api_key,
    "text": group,
  }
  url = "%s?%s" % (api_url, urllib.urlencode(data))
  xml = urllib.urlopen(url).read()
  doc = minidom.parseString(xml)
  return xpath.Evaluate("//group[0]/@nsid")[0]

def search(tags, sort, group=None):
  data = {
    "method": "flickr.photos.search",
    "api_key": api_key,
    "tags": tags,
    "sort": sort,
  }

  if group:
    data["group_id"] = get_group_id(group)

  url = "%s?%s" % (api_url, urllib.urlencode(data))
  photoxml = urllib.urlopen(url).read()
  doc = minidom.parseString(photoxml)
  for photo_el in xpath.Evaluate("//photo", doc):
    photo = Photo(photo_el)
    print photo.GetURL()

search(sys.argv[1], "interestingness-desc")
