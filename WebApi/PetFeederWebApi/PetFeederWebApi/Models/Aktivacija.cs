using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;
using System.Diagnostics.CodeAnalysis;

namespace PetFeederWebApi.Models {
    [Table("Aktivacije")]
    public class Aktivacija {
        [Key]
        public int AktivacijaID { get; set; }
        [ForeignKey(nameof(Uredjaj))]
        public int UredjajID { get; set; }
        public Uredjaj? Uredjaj { get; set; }
        [NotNull]
        public DateTime Vrijeme { get; set; }
    }
}
