USE [PetFeeder]
GO
/****** Object:  Table [dbo].[Aktivacije]    Script Date: 17. 6. 2025. 22:26:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Aktivacije](
	[UredjajID] [int] NOT NULL,
	[Vrijeme] [datetime] NOT NULL,
	[AktivacijaID] [int] IDENTITY(1,1) NOT NULL,
PRIMARY KEY CLUSTERED 
(
	[AktivacijaID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Korisnici]    Script Date: 17. 6. 2025. 22:26:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Korisnici](
	[KorisnikID] [int] IDENTITY(1,1) NOT NULL,
	[Email] [nvarchar](50) NULL,
	[Lozinka] [nvarchar](50) NULL,
PRIMARY KEY CLUSTERED 
(
	[KorisnikID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[KorisniciUredjaji]    Script Date: 17. 6. 2025. 22:26:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[KorisniciUredjaji](
	[KorisnikID] [int] NOT NULL,
	[UredjajID] [int] NOT NULL,
 CONSTRAINT [PK_KorisniciUredjaji] PRIMARY KEY CLUSTERED 
(
	[KorisnikID] ASC,
	[UredjajID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
/****** Object:  Table [dbo].[Uredjaji]    Script Date: 17. 6. 2025. 22:26:48 ******/
SET ANSI_NULLS ON
GO
SET QUOTED_IDENTIFIER ON
GO
CREATE TABLE [dbo].[Uredjaji](
	[UredjajID] [int] IDENTITY(1,1) NOT NULL,
	[Ime] [nvarchar](30) NULL,
	[MAC] [nvarchar](17) NULL,
	[Izbacivanja] [int] NULL,
	[ImaoObrokVrijeme] [datetime] NULL,
	[PosljednjiUpdateVrijeme] [datetime] NULL,
PRIMARY KEY CLUSTERED 
(
	[UredjajID] ASC
)WITH (PAD_INDEX = OFF, STATISTICS_NORECOMPUTE = OFF, IGNORE_DUP_KEY = OFF, ALLOW_ROW_LOCKS = ON, ALLOW_PAGE_LOCKS = ON, OPTIMIZE_FOR_SEQUENTIAL_KEY = OFF) ON [PRIMARY]
) ON [PRIMARY]
GO
ALTER TABLE [dbo].[Uredjaji] ADD  DEFAULT ((0)) FOR [Izbacivanja]
GO
ALTER TABLE [dbo].[Aktivacije]  WITH CHECK ADD  CONSTRAINT [FK_Aktivacije_Uredjaji] FOREIGN KEY([UredjajID])
REFERENCES [dbo].[Uredjaji] ([UredjajID])
GO
ALTER TABLE [dbo].[Aktivacije] CHECK CONSTRAINT [FK_Aktivacije_Uredjaji]
GO
ALTER TABLE [dbo].[KorisniciUredjaji]  WITH CHECK ADD FOREIGN KEY([KorisnikID])
REFERENCES [dbo].[Korisnici] ([KorisnikID])
GO
ALTER TABLE [dbo].[KorisniciUredjaji]  WITH CHECK ADD FOREIGN KEY([UredjajID])
REFERENCES [dbo].[Uredjaji] ([UredjajID])
GO
