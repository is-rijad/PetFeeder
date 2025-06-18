using System.Text.RegularExpressions;

namespace PetFeederWebApi {
    public static class Helper {
        public static bool ValidirajEmail(string email) {
            if (!string.IsNullOrEmpty(email)) {
                var regex = new Regex("^[\\w._-]+@[a-z]+\\.com$");
                return regex.IsMatch(email);
            }
            return false;
        }
        public static bool ValidirajLozinku(string lozinka) {
            if (!string.IsNullOrEmpty(lozinka)) {
                var regex = new Regex("^.*(?=.{8,})(?=.*[a-zA-Z])(?=.*\\d).*$");
                return regex.IsMatch(lozinka);
            }
            return false;
        }
        public static string GenerisiLozinku() {
            string charSet = "qwertzuioplkjhgfdsayxcvbnmQWERTZUIOPLKJHGFDSAYXCVBNM1234567890";
            int velicinaLozinke = 10;
            int velicinaSeta = charSet.Length;
            string lozinka = string.Empty;
            Random random = new Random();
            for (int i = 0; i < velicinaLozinke;  i++) {
                var rand = random.Next(0, velicinaSeta);
                lozinka += charSet[rand];
            }
            return lozinka;
        }
    }
}
