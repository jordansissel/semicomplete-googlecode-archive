class CreateFieldValues < ActiveRecord::Migration
  def self.up
    create_table :field_values do |t|
      t.string :field
      t.string :value
      t.integer :startpos
      t.integer :endpos

      t.integer :log_entry_id, :references => :log_entriess

      t.timestamps
    end
  end

  def self.down
    drop_table :field_values
  end
end
