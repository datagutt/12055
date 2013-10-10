package android.content.pm; 


import android.database.Cursor;
import android.database.CursorWrapper;

public class OppoBlankCursor extends CursorWrapper {

	public OppoBlankCursor(Cursor cursor) {
		super(cursor);
	}

	@Override
	public int getCount() {
		return 0;
	}

	@Override
	public boolean move(int offset) {
		return false;
	}

	@Override
	public boolean moveToFirst() {
		return false;
	}

	@Override
	public boolean moveToLast() {
		return false;
	}

	@Override
	public boolean moveToNext() {
		return false;
	}

	@Override
	public boolean moveToPosition(int position) {
		return false;
	}

	@Override
	public boolean moveToPrevious() {
		return false;
	}
}

