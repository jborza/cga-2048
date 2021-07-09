using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace spritegen
{
    static class Program
    {
        static void Main(string[] args)
        {
            //TODO parametrize
            string sprite = args[0];
            Bitmap bmp = (Bitmap)Bitmap.FromFile(sprite + ".png");
            int rows = bmp.Height;
            Console.Write("int sprite_{0}[{1}] = {{", sprite, rows);
            int white = -1;
            for (int y = 0; y < rows; y++)
            {
                int i = 0;
                Console.Write("0x");
                byte b = 0;
                for (int x = 4; x < 8; x++)
                    i = ExtractByte(bmp, white, y, i, x);
                Console.Write("{0:x2}", i);
                i = 0;
                for (int x = 0; x < 4; x++)
                    i = ExtractByte(bmp, white, y, i, x);
                Console.Write("{0:x2}", i);
                if (y < rows - 1)
                    Console.Write(",");
            }
            Console.WriteLine("};");
        }

        private static int ExtractByte(Bitmap bmp, int white, int y, int i, int x)
        {
            i <<= 2;
            var px = bmp.GetPixel(x, y);
            if (px.ToArgb() == white)
            {
                i |= 0x3;
            }

            return i;
        }
    }
}
