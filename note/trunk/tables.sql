CREATE SCHEMA notes;

CREATE TABLE notes.notes (
	id               SERIAL,
	date_created     TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
	date_modified    TIMESTAMP WITH TIME ZONE DEFAULT NOW(),
	contents         TEXT,
	subject          VARCHAR(100),
	shown            INTEGER DEFAULT 1,

	PRIMARY KEY (id)
);

CREATE INDEX shown_index ON notes.notes (shown);

CREATE TABLE notes.followups (
	parentid         INTEGER DEFAULT 0 REFERENCES notes.notes ON DELETE CASCADE,
	childid          INTEGER DEFAULT 0 REFERENCES notes.notes ON DELETE CASCADE
);

INSERT INTO notes.notes (id, shown) VALUES (0, 0);
