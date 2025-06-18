using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace PetFeederWebApi.Models {
    [Table("Korisnici")]
    public class Korisnik {
        [Key]
        public int KorisnikID { get; set; }
        public string Email { get; set; }
        public string Lozinka { get; set; }

    }
}
