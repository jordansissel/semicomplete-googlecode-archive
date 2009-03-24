#!/usr/bin/env ruby
# TODO(sissel): support nesting and array indexing in 'key'
# Converting the JSON object to xml and permitting xpath would be nice
# For example: foo/bar[0]/baz will fetch
# { "foo": { "bar" : [ { "baz": 3 }, foo ] } } 
# and get you: 3
# Or something...

require "yaml"
require "json"
require "time"
require "date"

# Backport Ruby 1.9 feature
class Time
  def self.strptime(datestr, format)
    d = Date._strptime(datestr, format)
    raise ArgumentError, "invalid strptime format - `#{format}'" unless d
    make_time(d[:year], d[:mon], d[:mday], d[:hour], d[:min], d[:sec],
              d[:sec_fraction], d[:zone], now)
  end
end

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
    #puts "Parsing #{input[@key]} with '#{@format}'"
    t = Time.strptime(input[@key], @format)
    return t
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
    @history = Hash.new { |h,k| h[k] = [] }
    @value = Hash.new { |h,k| h[k] = 0 }
  end

  def feed(input)
    return unless @keys.collect { |k| input.has_key?(k) }.reduce { |a,b| a & b }
    filtered_input = process(input)
    key = keygen(filtered_input)
    @value[key] += 1

    rd = ReactorData.new
    rd.input = input
    rd.timestamp = @timeparser ? @timeparser.parse(input) : Time.now
    @history[key] << rd

    # If we hit interval, react on this key exactly as if we had hit the
    # threshold.
    trimtime = rd.timestamp - @interval
    @history.each do |histkey,histdata|
      while histdata.length > 0 && histdata[0].timestamp < trimtime
        @value[histdata[0]] -= 1
        histdata.shift
      end
    end

    threshold_exceeded = ((@threshold > 0) and (@value[key] >= @threshold))
    #puts [threshold_exceeded, @threshold, @value[key]].join(" / ")
    if (threshold_exceeded)
      react(key, rd.timestamp)
    end
  end

  def react(key, timestamp=nil)
    timestamp = Time.now unless timestamp
    #puts "Threshold exceeded: #{@value[key]} > #{@threshold}"
    puts "#{@value[key]}: #{key.inspect} / #{timestamp}"
    #puts @action
    @value[key] = 0
    @history[key].clear
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
  obj = JSON.parse(line)
  # Skip unless this object has all keys present
  reactors.each { |r| r.feed(obj) }
end
