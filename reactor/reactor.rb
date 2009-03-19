#!/usr/bin/env ruby

require "yaml"

class Reactor
  attr_accessor :threshold
  attr_accessor :keys

  def initialize
    @history = []
    @value = Hash.new { |h,k| h[k] = 0 }
    #@conditions = []
  end

  def feed(input)
    key = key_str(input)
    @value[key] += 1
    @history << input
    react(input) if @value[key] >= @threshold
  end

  def react(input)
    key = key_str(input)
    puts "Threshold exceeded: #{@value[key]} > #{@threshold}"
    puts input.obj.inspect
    @value[key] = 0
  end

  def key_str(input)
    @keys.collect { |k| input.obj[k] }.join(" ")
  end
end

class ReactorInput
  attr_accessor :obj
  attr_accessor :timestamp

  def initialize(obj=nil, timestamp=nil)
    @obj = obj || Hash.new
    @timestamp = timestamp || Time.now
  end

end

config = YAML::load(File.new("reactor.yaml").read)

data = Hash.new { |h,k| h[k] = [] }

keys = config["key"].split(",")
action = config["action"]

reactor = Reactor.new
reactor.keys = keys
reactor.threshold = action["threshold"]

$stdin.each do |line|
  obj = YAML::load(line)
  # Skip unless this object has all keys present
  next unless keys.collect { |k| obj.has_key?(k) }.reduce { |a,b| a & b }

  # make a new object with only the 'keys' key:value pairs present.
  shortobj = Hash[*keys.collect { |k| [k, obj[k]] }.flatten ]

  input = ReactorInput.new(shortobj, Time.now)
  reactor.feed(input)
end
