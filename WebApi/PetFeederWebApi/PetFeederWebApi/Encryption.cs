namespace PetFeederWebApi
{
    public static class Encryption
    {
        private static char Enkriptuj(char unos)
        {
            int e = 11;
            int n = 323;
            int enkriptovano = 1;
            for (int i = e; i > 0; i--)
            {
                enkriptovano *= unos;
                enkriptovano %= n;
            }

            return (char)enkriptovano;
        }

        private static char Dekriptuj(char enkriptovano)
        {
            int d = 131;
            int n = 323;
            int plain = 1;
            for (int i = d; i > 0; i--)
            {
                plain *= enkriptovano;
                plain %= n;
            }

            return (char)plain;
        }

        public static string Encoder(string plain)
        {

            int velicina = plain.Length;
            string rezultat = string.Empty;
            for (int i = 0; i < velicina; i++)
            {
                rezultat += Enkriptuj(plain[i]);
            }

            return rezultat;
        }

        public static string Decoder(string encoded)
        {
            string plain = string.Empty;
            int velicina = encoded.Length;
            for (int i = 0; i < velicina; i++)
            {
                plain += (char)Dekriptuj(encoded[i]);
            }

            return plain;
        }
    }
}
