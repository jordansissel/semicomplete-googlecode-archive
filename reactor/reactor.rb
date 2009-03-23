#!/usr/bin/env ruby
# TODO(sissel): support nesting and array indexing in 'key'
# Converting the JSON object to xml and permitting xpath would be nice
# For example: foo/bar[0]/baz will fetch
# { "foo": { "bar" : [ { "baz": 3 }, foo ] } } 
# and get you: 3
# Or something...

require "yaml"
require "time"

class ReactorData
  attr_accessor :timestamp
  attr_accessor :input
end

class TimeParserError < RuntimeError; end

class TimeParser
  attr_accessor :format
  attr_accessor :key

  def initialize(format, key)
    @format = format
    @key = key
  end

  def parse(input)
    if !input.has_key?(@key)
      raise TimeParserError, "Input has no timestamp key '#{@key}'"
    end

    return Time.parse(input[@key], @format)
  end
end

class Reactor
  attr_accessor :action
  attr_accessor :interval
  attr_accessor :keys
  attr_accessor :threshold

  def initialize(config)
    @action = config["action"]
    @interval = config["interval"].to_i
    # TODO(sissel): Accept yaml sequence, too, for keys?
    @keys = config["key"].split(",")
    @threshold = config["threshold"].to_i

    if config.has_key?("timestamp-key")
      @timeparser = TimeParser.new(config["timestamp-format"],
                                   config["timestamp-key"])
    else
      @timeparser = nil
    end

    #@history = HistoryTracker.new
    @history = []
    @value = Hash.new { |h,k| h[k] = 0 }
  end

  def feed(input)
    return unless @keys.collect { |k| input.has_key?(k) }.reduce { |a,b| a & b }
    input = process(input)
    key = keygen(input)
    @value[key] += 1

    rh = ReactorData.new
    rh.input = input
    rh.timestamp = @timeparser ? @timeparser.parse(input) : Time.now
    @history << rh
    # If the first history entry older than the age range starting at 
    # rh.timestamp (ending in rh.timestmap - @interval), delete it.
    oldrh = @history[0]

    # Special case. If we have no threshold but have an interval,
    # If we hit interval, react on this key exactly as if we had hit the
    # threshold.
    if oldrh.timestamp < (rh.timestamp - @interval)
      oldkey = keygen(oldrh.input)
      @value[oldkey] -= 1
      @history.shift
    end

    react(key) if @value[key] >= @threshold
  end

  def react(key)
    puts "Threshold exceeded: #{@value[key]} > #{@threshold}"
    puts key.inspect
    @value[key] = 0
  end

  def process(input)
    # make a new object with only the 'keys' key:value pairs present.
    input = Hash[*keys.collect { |k| [k, input[k]] }.flatten ]
    return input
  end

  def keygen(input)
    #@keys.collect { |k| input[k] }
    input
  end
end

config = YAML::load(File.new("reactor.yaml").read)

data = Hash.new { |h,k| h[k] = [] }

reactors = []
config.each do |c|
  r = Reactor.new(c)
  reactors << r
end

$stdin.each do |line|
  obj = YAML::load(line)
  # Skip unless this object has all keys present
  reactors.each { |r| r.feed(obj) }
end
