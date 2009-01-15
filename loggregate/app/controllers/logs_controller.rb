class LogsController < ApplicationController
  #def index
    #logs = Logs.find(:all, :limit => 10)
    #log_lines = []
    #fields = Hash.new
    #logs.each do |log|
      #line = log.value
      #fields[log.id] = FieldValues.find_by_log_id(log.id)
    #end
  #end
  
  def show
    result = FieldValue.find_all_by_field_and_value(
        params[:field], params[:value],
        :select => "DISTINCT log_entry_id")
    
    @logs = LogEntry.find(result.collect { |r| r.log_entry_id })
    respond_to do |format|
      format.html
    end
  end

  def index
    @fields = FieldValue.find(:all, :select => "DISTINCT field") \
      .collect { |f| f.field } \
      .select { |f| !f.match(":") }

    @recent = LogEntry.find(:all, :limit => 20)
  end
end
