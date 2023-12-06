
import NonFungibleToken from 0x1d7e57aa55817448
import MetadataViews from 0x1d7e57aa55817448

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

              var displayInfo : {String:String} = {"bannerImage":"", "description":"", "identifier":name,  "name":type.identifier.slice(from: 19, upTo: type.identifier.length), "count":collection.getIDs().length.toString()}

              var id = collection.getIDs()[0]
              var nft = collection.borrowNFT(id: id)

              if let data = nft.resolveView(Type<MetadataViews.NFTCollectionDisplay>()){
                if let display = data as? MetadataViews.NFTCollectionDisplay{
                    displayInfo["name"] = display.name
                    displayInfo["description"] =  display.description
                    displayInfo["bannerImage"] = display.squareImage.file.uri()
                }
              }
              ft.append(displayInfo)

            }
            return true
          })


  return ft
}
