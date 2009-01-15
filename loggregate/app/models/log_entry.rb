class LogEntry < ActiveRecord::Base
  has_many :field_values
end
