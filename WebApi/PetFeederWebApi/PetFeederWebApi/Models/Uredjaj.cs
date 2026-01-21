using System.ComponentModel.DataAnnotations;
using System.ComponentModel.DataAnnotations.Schema;

namespace PetFeederWebApi.Models
{
    [Table("Uredjaji")]
    public class Uredjaj
    {
        [Key]
        public int UredjajID { get; set; }
        public string Ime { get; set; }
        public string Mac { get; set; }
        public int Izbacivanja { get; set; } = 0;
        public DateTime? ImaoObrokVrijeme { get; set; }
    }
}
