using System;


public class HmSelctedTextAutoHighlighter
{
	public static IntPtr Start()
	{
		IntPtr result;
		try
		{
			if (HmSelctedTextAutoHighlighterForm.form == null)
			{
				HmSelctedTextAutoHighlighterForm.form = new HmSelctedTextAutoHighlighterForm();
			}
			result = (IntPtr)1;
		}
		catch (Exception)
		{
			result = (IntPtr)0;
		}
		return result;
	}

	public static IntPtr OnDetachMethod()
	{
		IntPtr result;
		try
		{
			if (HmSelctedTextAutoHighlighterForm.form != null)
			{
				HmSelctedTextAutoHighlighterForm.form.Stop();
				HmSelctedTextAutoHighlighterForm.form = null;
			}
			result = (IntPtr)1;
		}
		catch (Exception)
		{
			result = (IntPtr)0;
		}
		return result;
	}
}
