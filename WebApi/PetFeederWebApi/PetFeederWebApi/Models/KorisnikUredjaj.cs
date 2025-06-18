using Microsoft.EntityFrameworkCore;
using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace PetFeederWebApi.Models {
    [Table("KorisniciUredjaji")]
    public class KorisnikUredjaj {
        [ForeignKey(nameof(Korisnik))]
        public int KorisnikID { get; set; }
        public Korisnik Korisnik { get; set; }
        [ForeignKey(nameof(Uredjaj))]
        public int UredjajID { get; set; }
        public Uredjaj Uredjaj { get; set; }

    }
}
