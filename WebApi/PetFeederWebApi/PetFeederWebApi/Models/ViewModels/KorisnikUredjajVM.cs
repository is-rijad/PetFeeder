namespace PetFeederWebApi.Models.ViewModels {
    public class KorisnikUredjajVM {
        public int korisnikID { get; set; }
        public string uredjajIme { get; set; }
        public string? uredjajMac { get; set; } = null;
    }
}
