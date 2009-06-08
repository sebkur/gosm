package pdf;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.MalformedURLException;

import com.lowagie.text.Document;
import com.lowagie.text.DocumentException;
import com.lowagie.text.Image;
import com.lowagie.text.Rectangle;
import com.lowagie.text.pdf.PdfWriter;

public class Exporter
{

	private static double INCH = 25.4;
	private static double DEFAULT_DPI = 72;

	public static void main(String[] args)
	{
		// cmd width height dpi left bottom output in1 in2 ... inN

		if (args.length < 7) {
			System.out
					.println("cmd width height dpi left bottom output in1 in2 ... inN");
			System.exit(1);
		}

		int pageWidth = 210;
		int pageHeight = 297;
		int dpi = 72;
		int borderLeft = 15;
		int borderBottom = 15;
		String output = "test.pdf";

		int a = 0;
		pageWidth = Integer.valueOf(args[a++]);
		pageHeight = Integer.valueOf(args[a++]);
		dpi = Integer.valueOf(args[a++]);
		borderLeft = Integer.valueOf(args[a++]);
		borderBottom = Integer.valueOf(args[a++]);
		output = args[a++];

		float bLeft = (float) (borderLeft / INCH * DEFAULT_DPI);
		float bBottom = (float) (borderBottom / INCH * DEFAULT_DPI);

		Document document = new Document();
		try {
			PdfWriter.getInstance(document, new FileOutputStream(output));
			document.setPageSize(new Rectangle(
					(float) (pageWidth / INCH * DEFAULT_DPI),
					(float) (pageHeight / INCH * DEFAULT_DPI)));
			document.open();

			for (int i = a; i < args.length; i++) {
				if (i > a) {
					document.newPage();
				}
				Image image = Image.getInstance(args[i]);
				image.scalePercent((float) (DEFAULT_DPI / dpi * 100));
				image.setAbsolutePosition(bLeft, bBottom);
				document.add(image);
				System.out.println("1");
			}
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		} catch (DocumentException e) {
			e.printStackTrace();
		} catch (MalformedURLException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		document.close();
	}
}
