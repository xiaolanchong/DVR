CREATE TABLE [dbo].[ComputersTable] (
	[ComputerID] [int] IDENTITY (1, 1) NOT NULL ,
	[ComputerIP] [varchar] (20) COLLATE Cyrillic_General_CI_AS NOT NULL ,
	[ArchivePath] [varchar] (255) COLLATE Cyrillic_General_CI_AS NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Devices] (
	[DeviceID] [int] IDENTITY (1, 1) NOT NULL ,
	[ComputerID] [int] NOT NULL ,
	[DeviceUID] [varchar] (50) COLLATE Cyrillic_General_CI_AS NOT NULL ,
	[Device_DisplayName] [varchar] (1024) COLLATE Cyrillic_General_CI_AS NULL ,
	[Device_BWMode] [int] NOT NULL ,
	[Device_FrameRate] [int] NOT NULL ,
	[Device_Width] [int] NOT NULL ,
	[Device_Height] [int] NOT NULL ,
	[Device_Standard] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[System] (
	[Codec_DataRate] [int] NOT NULL ,
	[Codec_FCC] [int] NOT NULL ,
	[Codec_KeyRate] [int] NOT NULL ,
	[Codec_Quality] [int] NOT NULL ,
	[Codec_State] [image] NULL ,
	[Video_FMP] [int] NOT NULL ,
	[Video_MaxSize] [int] NOT NULL ,
	[AlgorithmSensitivity] [int] NOT NULL ,
	[ArchiveWriteCondition] [int] NOT NULL ,
	[FreePercentageDiskSpace] [int] NOT NULL ,
	[StorageDayPeriod] [int] NOT NULL 
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Cameras] (
	[CameraID] [int] IDENTITY (1, 1) NOT NULL ,
	[DeviceID] [int] NOT NULL ,
	[PinNumber] [int] NOT NULL ,
	[Description] [varchar] (256) COLLATE Cyrillic_General_CI_AS
) ON [PRIMARY]
GO

CREATE TABLE [dbo].[Frames] (
	[FrameID] [int] IDENTITY (1, 1) NOT NULL ,
	[CameraID] [int] NOT NULL ,
	[TimeBegin] [datetime] NOT NULL ,
	[TimeEnd] [datetime] NOT NULL 
) ON [PRIMARY]
GO

ALTER TABLE [dbo].[ComputersTable] WITH NOCHECK ADD 
	CONSTRAINT [PK_Computers] PRIMARY KEY  CLUSTERED 
	(
		[ComputerID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Devices] WITH NOCHECK ADD 
	CONSTRAINT [PK_Devices] PRIMARY KEY  CLUSTERED 
	(
		[DeviceID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Cameras] WITH NOCHECK ADD 
	CONSTRAINT [PK_Cameras] PRIMARY KEY  CLUSTERED 
	(
		[CameraID]
	)  ON [PRIMARY] 
GO

ALTER TABLE [dbo].[Frames] WITH NOCHECK ADD 
	CONSTRAINT [PK_Frames] PRIMARY KEY  CLUSTERED 
	(
		[FrameID]
	)  ON [PRIMARY] 
GO


ALTER TABLE [dbo].[Devices] ADD 
	CONSTRAINT [DF_Devices_Device_BWMode] DEFAULT (0) FOR [Device_BWMode],
	CONSTRAINT [DF_Devices_Device_FrameRate] DEFAULT (2) FOR [Device_FrameRate],
	CONSTRAINT [DF_Devices_Device_Width] DEFAULT (352) FOR [Device_Width],
	CONSTRAINT [DF_Devices_Device_Height] DEFAULT (288) FOR [Device_Height],
	CONSTRAINT [DF_Devices_Device_Standard] DEFAULT (16) FOR [Device_Standard]
GO

ALTER TABLE [dbo].[System] ADD 
	CONSTRAINT [DF_System_Codec_DataRate] DEFAULT (0) FOR [Codec_DataRate],
	CONSTRAINT [DF_System_Codec_FCC] DEFAULT (1145656920) FOR [Codec_FCC],
	CONSTRAINT [DF_System_Codec_KeyRate] DEFAULT (0) FOR [Codec_KeyRate],
	CONSTRAINT [DF_System_Codec_Quality] DEFAULT (10000) FOR [Codec_Quality],
	CONSTRAINT [DF_System_Video_FMP] DEFAULT (120) FOR [Video_FMP],
	CONSTRAINT [DF_System_Video_MaxSize] DEFAULT (300) FOR [Video_MaxSize],
	CONSTRAINT [DF_System_AlgorithmSensitivity] DEFAULT (50) FOR [AlgorithmSensitivity],
	CONSTRAINT [DF_System_ArchiveWriteCondition] DEFAULT (2) FOR [ArchiveWriteCondition],
	CONSTRAINT [DF_System_FreePercentageDiskSpace] DEFAULT (15) FOR [FreePercentageDiskSpace],
	CONSTRAINT [DF_System_StorageDayPeriod] DEFAULT (21) FOR [StorageDayPeriod]
GO

ALTER TABLE [dbo].[Cameras] ADD 
	CONSTRAINT [FK_Cameras_Devices] FOREIGN KEY 
	(
		[DeviceID]
	) REFERENCES [dbo].[Devices] (
		[DeviceID]
	) ON DELETE CASCADE  ON UPDATE CASCADE 
GO

ALTER TABLE [dbo].[Frames] ADD 
	CONSTRAINT [FK_Frames_Cameras] FOREIGN KEY 
	(
		[CameraID]
	) REFERENCES [dbo].[Cameras] (
		[CameraID]
	) ON DELETE CASCADE ON UPDATE CASCADE 
GO

SET QUOTED_IDENTIFIER ON 
GO
SET ANSI_NULLS ON 
GO

CREATE VIEW dbo.Computers
AS
SELECT     dbo.ComputersTable.ComputerID, dbo.ComputersTable.ComputerIP, dbo.ComputersTable.ArchivePath, dbo.System.FreePercentageDiskSpace, 
                      dbo.System.StorageDayPeriod, dbo.System.Codec_DataRate, dbo.System.Codec_FCC, dbo.System.Codec_KeyRate, 
                      dbo.System.Codec_Quality, dbo.System.Codec_State, dbo.System.Video_FMP, dbo.System.Video_MaxSize, dbo.System.AlgorithmSensitivity, 
                      dbo.System.ArchiveWriteCondition
FROM         dbo.ComputersTable CROSS JOIN
                      dbo.System

GO
SET QUOTED_IDENTIFIER OFF 
GO
SET ANSI_NULLS ON 
GO

INSERT INTO [System](Codec_State) VALUES(NULL)
GO
