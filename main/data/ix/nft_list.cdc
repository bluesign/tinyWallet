
import NonFungibleToken from 0x1d7e57aa55817448
import MetadataViews from 0x1d7e57aa55817448
import Icons from 0x5f195aa4322e5dbb

pub fun main(addr: Address):  AnyStruct {
    let acct = getAccount(addr)

    var ft : [AnyStruct] = []

    getAuthAccount(addr).forEachStored(fun (path: StoragePath, type: Type): Bool {
        if type.isSubtype(of: Type<@NonFungibleToken.Collection>()){

        var collection = getAuthAccount(addr).borrow<&NonFungibleToken.Collection>(from:path)!
        if (collection.getIDs().length==0){
            return true
        }

        var name = type.identifier.slice(from: 19, upTo: type.identifier.length);
        if name.length>6{
            if name.slice(from: name.length-11, upTo:name.length)==".Collection"{
                 name = name.slice(from: 0, upTo: name.length-11)
            }
        }

        var idf = type.identifier
        if idf.slice(from: idf.length-11, upTo:idf.length)==".Collection"{
             idf = idf.slice(from: 0, upTo: idf.length-11)
        }

        var image = "iVBORw0KGgoAAAANSUhEUgAAADAAAAAwBAMAAAClLOS0AAAAHlBMVEVHcEwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABaVcReAAAACXRSTlMAzBHdIojuRGYxMAAkAAABSUlEQVQ4y6WUS0/DMAyAo6jSehwbo+utjYTgOg0kepuAiSsTL3GcEIjcUgHiOh5/IIoWyf8WB1iTZfUFcnAUf3HsurYZaxb/OCpY26qlzdr0UwlgL1v0FQzfwGyQ/RwOJ8kF6Pd1fWcH7ieMJdcwV6EeFVoFF3ygzRP4pA+a14FTDCJbkRdpb7y5P4V3Qvs0fHXlcYF7uRbHb4w93E6Mir+2Ux04oDdyyvO/A1EwzpgQuDvRgLS/zWcq7Y/Hw10UWQNKWCZVtwQA/YRiGQBzKhGMttAC5qEFfCIwx+hjT/aKEGQOuAwksuujKm31iMAOrooImFstv50vYoDBIHi4UzGYOmBfn2Mf5kxCq3Mtcg/OPUgHrB4plC6BMxUkEXMoGPvJMv/f/2gHVDGQ5UMWHFmidFHTbUA2Dt1qdHPS7UwPAHpk0EMmGktf0SqYqmsyBKMAAAAASUVORK5CYII="
        if let img = Icons.icons[idf]{
            image = img
        }

        var displayInfo : {String:String} = {
            "image":image,
            "name": name,
            "count":collection.getIDs().length.toString()
        }

        var id = collection.getIDs()[0]
        var nft = collection.borrowNFT(id: id)

        if let data = nft.resolveView(Type<MetadataViews.NFTCollectionDisplay>()){
            if let display = data as? MetadataViews.NFTCollectionDisplay{
                displayInfo["name"] = display.name
            }
        }

        ft.append(displayInfo)

       }
       return true
    })

  return ft
}
