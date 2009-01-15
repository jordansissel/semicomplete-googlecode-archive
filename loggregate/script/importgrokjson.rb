#!/usr/bin/env /home/jls/projects/loggregate/script/runner
#

require "rubygems"
gem "json"
require "json"
require "pp"

$stdin.each do |data|
  obj = JSON.parse(data)
  entry = Hash.new { |h,k| h[k] = [] }
  obj.each do |field|
    name = field.keys[0]
    entry[name] << {
      :pos_start => field[name]["start"],
      :pos_end => field[name]["end"],
      :value => field[name]["value"],
    }
  end

  #puts entry["@LINE"][0][:value]
  l = LogEntry.new
  l.value = entry["@LINE"][0][:value]
  l.save

  entry.each do |key,values|
    names = [key]

    # Add 'foo' for a key of 'NUMBER:foo'
    names << key.split(":")[-1] if key[/:/]

    names.each do |name|
      values.each do |e|
        fv = FieldValue.new
        fv.log_entry_id = l.id
        fv.field = name.downcase
        fv.value = e[:value]
        fv.startpos = e[:pos_start]
        fv.endpos = e[:pos_end]
        fv.save
      end
    end
  end

  puts "Processed: #{l.value}"
end
